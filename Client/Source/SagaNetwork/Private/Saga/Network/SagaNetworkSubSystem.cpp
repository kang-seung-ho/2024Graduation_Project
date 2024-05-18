#include "Saga/Network/SagaNetworkSubSystem.h"
#include <cstddef>
#include <Sockets.h>

#include "Saga/Network/SagaBasicPacket.h"

USagaNetworkSubSystem::USagaNetworkSubSystem()
	: UGameInstanceSubsystem()
	, localUserId(-1), localUserName(), currentRoomId(), currentRoomTitle(), currentHandledWeapon()
	, OnNetworkInitialized(), OnConnected(), OnFailedToConnect(), OnDisconnected(), OnSignedIn()
	, OnRoomCreated(), OnJoinedRoom(), OnOtherJoinedRoom(), OnLeftRoomBySelf(), OnLeftRoom()
	, OnRespondVersion(), OnUpdateRoomList(), OnUpdateMembers()
	, OnTeamChanged()
	, OnGetPreparedGame(), OnStartGame(), OnCreatingCharacter()
	, OnUpdatePosition(), OnUpdateRotation(), OnRpc()
	, clientSocket(), netWorker(), taskWorker()
	, netIsOfflineMode(false), netConnectOption(), netRemoteAddress(), netRemotePort(), netLocalPort()
	, recvBuffer(), recvBytes(), transitBuffer(), transitOffset()
	, receivedDataLock()
	, everyUsers(), everyRooms(), wasUsersUpdated(true), wasRoomsUpdated(true)
{}

ESagaConnectionContract
USagaNetworkSubSystem::ConnectToServer(const FName& nickname)
{
	if (not IsOfflineMode())
	{
		if (IsConnected())
		{
			UE_LOG(LogSagaNetwork, Warning, TEXT("The network subsystem is getting started."));

			return ESagaConnectionContract::Success;
		}
		else
		{
			if (not IsSocketAvailable())
			{
				UE_LOG(LogSagaNetwork, Warning, TEXT("The socket was not initialized."));

				return ESagaConnectionContract::SubSystemError;
			}

			USagaNetworkSubSystem::SetLocalUserName(nickname);

			UE_LOG(LogSagaNetwork, Log, TEXT("Connecting..."));

			auto connect_r = ConnectToServer_Implementation();
			if (connect_r == ESagaConnectionContract::Success)
			{
				// 서버가 닉네임 패킷을 받으면 서버는 ID 부여 패킷을 보낸다.
				// 클라는 ID 부여 패킷을 받아서 갱신하고, 게임 or 메뉴 레벨로 넘어가야 한다.
				BroadcastOnConnected();

				return connect_r;
			}
			else
			{
				auto str = UEnum::GetValueAsString(connect_r);

				UE_LOG(LogSagaNetwork, Error, TEXT("Cannot start the network subsystem!"));

				BroadcastOnFailedToConnect(connect_r);

				return connect_r;
			}
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("The network subsystem is getting started. (Offline Mode)"));

		return ESagaConnectionContract::Success;
	}
}

bool
USagaNetworkSubSystem::Close()
{
	if (not IsOfflineMode())
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
	if (IsOfflineMode())
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("[Packet] Receiving... (Offline Mode)"));

		return true;
	}
	else // IF (IsOfflineMode)
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("[Packet] Receiving..."));

		auto recv_buffer = recvBuffer.GetData();
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
				{
					FScopeLock locker{ &receivedDataLock };

					const auto dst = transitBuffer.GetData() + transitOffset;
					std::memcpy(dst, read_buffer, static_cast<size_t>(basic_pk.mySize));
					transitOffset += basic_pk.mySize;
				}

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
	} // IF NOT (IsOfflineMode)

	return true;
}

bool
USagaNetworkSubSystem::ProcessPackets()
{
	int32 process_offset{};

	while (0 < transitOffset)
	{
		// Process a packet
		{
			FScopeLock locker{ &receivedDataLock };

			auto process_buffer = transitBuffer.GetData() + process_offset;

			FSagaBasicPacket basic_pk{ EPacketProtocol::UNKNOWN };
			basic_pk.Read(reinterpret_cast<std::byte*>(process_buffer));

			if (basic_pk.mySize <= 0 or transitOffset < basic_pk.SignedMinSize())
			{
				break;
			}

			// Routes stacked events
			UE_LOG(LogSagaNetwork, Log, TEXT("[Packet] Routing packet events..."));

			RouteEvents(transitBuffer, process_offset, basic_pk.myProtocol, basic_pk.mySize);

			process_offset += basic_pk.mySize;
			transitOffset -= basic_pk.mySize;
		}
	}

	return IsConnected();
}

const TArray<FSagaVirtualUser>&
USagaNetworkSubSystem::UpdatePlayerList()
{
	// TODO: UpdatePlayerList
	return everyUsers;
}

const TArray<FSagaVirtualRoom>&
USagaNetworkSubSystem::UpdateRoomList()
{
	// TODO: UpdateRoomList
	return everyRooms;
}

ESagaNetworkConnectionCategory
USagaNetworkSubSystem::GetConnectionOption()
const noexcept
{
	return netConnectOption;
}

FString
USagaNetworkSubSystem::GetRemoteAddress()
const
{
	return netRemoteAddress;
}

int32
USagaNetworkSubSystem::GetRemotePort()
const noexcept
{
	return netRemotePort;
}

int32
USagaNetworkSubSystem::GetLocalPort()
const noexcept
{
	return netLocalPort;
}
