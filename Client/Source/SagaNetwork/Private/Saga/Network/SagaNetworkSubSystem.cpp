#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Sockets.h"
#include <cstddef>

#include "Saga/Network/SagaNetworkSettings.h"
#include "Saga/Network/SagaBasicPacket.h"
#include "SagaGame/Character/SagaCharacterPlayer.h"
#include "SagaGame/Character/SagaPlayableCharacter.h"

USagaNetworkSubSystem::USagaNetworkSubSystem()
	: UGameInstanceSubsystem()
	, localUserId(-1), localUserName(), currentRoomId(), currentRoomTitle()
	, OnNetworkInitialized(), OnConnected(), OnFailedToConnect(), OnDisconnected()
	, OnRoomCreated(), OnJoinedRoom(), OnOtherJoinedRoom(), OnLeftRoomBySelf(), OnLeftRoom()
	, OnRespondVersion(), OnUpdateRoomList(), OnUpdateMembers()
	, OnTeamChanged()
	, OnGetPreparedGame(), OnStartGame()
	, OnUpdatePosition(), OnUpdateRotation(), OnCreatingCharacter()
	, clientSocket(nullptr), netWorker(nullptr)
	, recvBuffer(), recvBytes(), transitBuffer(), transitOffset()
	, everyUsers(), everyRooms(), wasUsersUpdated(true), wasRoomsUpdated(true)
	, localPlayerClassReference(), dummyPlayerClassReference()
{
	static ConstructorHelpers::FClassFinder<AActor> character_class_seek1(TEXT("/Script/CoreUObject.Class'/Script/SagaGame.SagaPlayableCharacter'"));
	if (character_class_seek1.Succeeded() and character_class_seek1.Class)
	{
		localPlayerClassReference = character_class_seek1.Class;
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame] Could not find class of the playable character"));
	}

	dummyPlayerClassReference = ASagaPlayableCharacter::StaticClass();
}

bool
USagaNetworkSubSystem::ConnectToServer(const FString & nickname)
{
	if constexpr (not saga::IsOfflineMode)
	{
		if (IsConnected())
		{
			UE_LOG(LogSagaNetwork, Warning, TEXT("The network subsystem had been started."));
			return true;
		}
		else
		{
			if (not IsSocketAvailable())
			{
				UE_LOG(LogSagaNetwork, Warning, TEXT("The network subsystem was not initialized."));

				if (not InitializeNetwork_Implementation())
				{
					UE_LOG(LogSagaNetwork, Error, TEXT("The network subsystem could not be initialized!"));

					return false;
				}
			}

			USagaNetworkSubSystem::SetLocalUserName(nickname);

			UE_LOG(LogSagaNetwork, Log, TEXT("Connecting..."));

			auto connect_r = ConnectToServer_Implementation();
			if (connect_r == ESagaConnectionContract::Success)
			{
				// #4
				// 서버가 닉네임 패킷을 받으면 서버는 ID 부여 패킷을 보낸다.
				// 클라는 ID 부여 패킷을 받아서 갱신하고, 게임 or 메뉴 레벨로 넘어가야 한다.
				UE_LOG(LogSagaNetwork, Log, TEXT("The network subsystem is started."));

				return true;
			}
			else
			{
				auto str = UEnum::GetValueAsString(connect_r);

				UE_LOG(LogSagaNetwork, Error, TEXT("Cannot start the network subsystem!"));
				UE_LOG(LogSagaNetwork, Error, TEXT("Could not connect to the server, due to `%s`"), *str);

				BroadcastOnFailedToConnect(connect_r);
				return false;
			}
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("The network subsystem is started. (Offline Mode)"));

		return true;
	}
}

bool
USagaNetworkSubSystem::Close()
{
	if constexpr (not saga::IsOfflineMode)
	{
		if (not IsSocketAvailable())
		{
			UE_LOG(LogSagaNetwork, Warning, TEXT("The socket of client is null."));
			return true;
		}
		else if (not IsConnected())
		{
			UE_LOG(LogSagaNetwork, Warning, TEXT("The network subsystem had been closed."));

			return CloseNetwork_Implementation();
		}
		else
		{
			UE_LOG(LogSagaNetwork, Warning, TEXT("Closing the network subsystem..."));

			return CloseNetwork_Implementation();
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Warning, TEXT("Closing the network subsystem... (Offline Mode)"));
		return true;
	}
}

bool
USagaNetworkSubSystem::Receive()
{
	auto recv_buffer = recvBuffer.GetData();

	if constexpr (not saga::IsOfflineMode)
	{
		int32 read_bytes{};

		// Checks
		if (not clientSocket->Recv(recv_buffer + recvBytes
			, recvLimit - recvBytes
			, read_bytes))
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[Packet] Receiving has been failed!"));
			BroadcastOnDisconnected();

			// Return #1
			return false;
		}
		else if (read_bytes <= 0)
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[Packet] Received %d byte!"), read_bytes);
			BroadcastOnDisconnected();

			// Return #2
			return false;
		}

		// Increases the bytes
		recvBytes += read_bytes;

		int32 read_offset{};

		// Peeks packets as many as possible
		while (FSagaBasicPacket::MinSize() <= recvBytes)
		{
			// The current state buffer
			const auto read_buffer = recv_buffer + read_offset;

			// Reads the protocol and size
			FSagaBasicPacket basic_pk{ EPacketProtocol::UNKNOWN };
			basic_pk.Read(reinterpret_cast<std::byte*>(read_buffer));

			// Validates
			if (basic_pk.mySize <= 0)
			{
				UE_LOG(LogSagaNetwork, Error, TEXT("[Packet] Packet's size was %d!"), basic_pk.mySize);

				// Return #3
				return false;
			}
			else if (basic_pk.mySize <= recvBytes)
			{
#if WITH_EDITOR
				const auto ename = UEnum::GetValueAsString(basic_pk.myProtocol);
				UE_LOG(LogSagaNetwork, Log, TEXT("[Packet] Received a packet (%s, %d)."), *ename, basic_pk.mySize);
#endif

				// Peeks a packet
				const auto dst = transitBuffer.GetData() + transitOffset;
				std::memcpy(dst, read_buffer, static_cast<size_t>(basic_pk.mySize));
				transitOffset += basic_pk.mySize;

				recvBytes -= basic_pk.mySize;
				read_offset += basic_pk.mySize;
			}
			else
			{
				UE_LOG(LogSagaNetwork, Log, TEXT("[Packet] A receive phase is done."));

				break;
			}
		} // WHILE (true)

		if (0 < read_offset)
		{
			const auto adv_buffer = recv_buffer + read_offset;
			const auto remained_size = static_cast<size_t>(recvLimit - read_offset);
			std::memcpy(recv_buffer, recv_buffer + read_offset, remained_size);
			std::memset(adv_buffer, 0, remained_size);
			UE_LOG(LogSagaNetwork, Log, TEXT("[Packet] Buffer is pulled by %d bytes"), read_offset);
		}

		int32 process_offset{};

		while (0 < transitOffset)
		{
			auto process_buffer = transitBuffer.GetData() + process_offset;

			FSagaBasicPacket basic_pk{ EPacketProtocol::UNKNOWN };
			basic_pk.Read(reinterpret_cast<std::byte*>(process_buffer));

			// Routes stacked events
			RouteEvents(transitBuffer, process_offset, basic_pk.myProtocol, basic_pk.mySize);

			process_offset += basic_pk.mySize;
			transitOffset -= basic_pk.mySize;
		}

	} // IF constexpr (IsOfflineMode)

	return true;
}

AActor*
USagaNetworkSubSystem::CreatePlayableCharacter(UClass * type, const FTransform & transform)
const
{
	if (type != nullptr)
	{
		FActorSpawnParameters setting{};
		setting.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		UE_LOG(LogSagaNetwork, Log, TEXT("[SagaGame] Creating a playable character"));
		return GetWorld()->SpawnActor(type, &transform, MoveTempIfPossible(setting));
	}
	else
	{
		return nullptr;
	}
}

USagaNetworkSubSystem*
USagaNetworkSubSystem::GetSubSystem(const UWorld * world) noexcept
{
	auto singleton = world->GetGameInstance();

	return singleton->GetSubsystem<USagaNetworkSubSystem>();
}

const TArray<FSagaVirtualUser>&
USagaNetworkSubSystem::UpdatePlayerList()
{
	// TODO: UpdatePlayerList
	//TAtomic<bool> a;
	return everyUsers;
}

const TArray<FSagaVirtualRoom>&
USagaNetworkSubSystem::UpdateRoomList()
{
	// TODO: UpdateRoomList
	return everyRooms;
}

bool
USagaNetworkSubSystem::IsSocketAvailable()
const noexcept
{
	return nullptr != clientSocket;
}

bool
USagaNetworkSubSystem::IsConnected()
const noexcept
{
	if (clientSocket != nullptr and clientSocket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
	{
		return true;
	}
	else
	{
		return false;
	}
}
