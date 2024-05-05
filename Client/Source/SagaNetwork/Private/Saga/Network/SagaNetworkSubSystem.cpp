#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Containers/Queue.h"
#include "Containers/Map.h"
#include "Sockets.h"

#include "Saga/Network/SagaNetworkSettings.h"
#include "SagaGame/Character/SagaCharacterPlayer.h"

TQueue<UE::Tasks::TTask<int32>> USagaNetworkSubSystem::taskQueue{};
//TMap<FStringView, TUniqueFunction<void()>> USagaNetworkSubSystem::rpcDatabase{};

void
USagaNetworkSubSystem::SetLocalUserId(int32 id)
noexcept
{
	localUserId = id;
}

int32
USagaNetworkSubSystem::GetLocalUserId()
const noexcept
{
	return localUserId;
}

void
USagaNetworkSubSystem::SetLocalUserName(const FString& nickname)
{
	localUserName = nickname;
}

FString
USagaNetworkSubSystem::GetLocalUserName()
const
{
	return localUserName;
}

bool
USagaNetworkSubSystem::GetLocalUserTeam(EUserTeam& outpin)
noexcept
{
	return GetTeam(localUserId, outpin);
}

void
USagaNetworkSubSystem::SetCurrentRoomId(int32 id)
noexcept
{
	currentRoomId = id;
}

int32
USagaNetworkSubSystem::GetCurrentRoomId()
const noexcept
{
	return currentRoomId;
}

void
USagaNetworkSubSystem::SetCurrentRoomTitle(const FString& title)
{
	currentRoomTitle = title;
}

FString
USagaNetworkSubSystem::GetCurrentRoomTitle()
const
{
	return currentRoomTitle;
}

void
USagaNetworkSubSystem::AddUser(const FSagaVirtualUser& client)
{
	everyUsers.Add(client);
	wasUsersUpdated = true;
}

bool
USagaNetworkSubSystem::FindUser(int32 id, FSagaVirtualUser& outpin)
const noexcept
{
	auto handle = everyUsers.FindByPredicate(FSagaSessionIdComparator{ id });
	if (nullptr != handle)
	{
		outpin = *handle;
		return true;
	}
	else
	{
		return false;
	}
}

bool
USagaNetworkSubSystem::RemoveUser(int32 id)
noexcept
{
	const bool result = 0 < everyUsers.RemoveAllSwap(FSagaSessionIdComparator{ id });
	if (result)
	{
		wasUsersUpdated = true;
	}

	return result;
}

void
USagaNetworkSubSystem::ClearUserList()
noexcept
{
	everyUsers.Reset();
	wasUsersUpdated = true;
}

bool
USagaNetworkSubSystem::GetTeam(int32 user_id, EUserTeam& outpin)
noexcept
{
	FSagaVirtualUser user{};

	if (FindUser(user_id, user))
	{
		outpin = user.myTeam;
		return true;
	}
	else
	{
		return false;
	}
}

void
USagaNetworkSubSystem::AddRoom(const FSagaVirtualRoom& room)
{
	everyRooms.Add(room);
	wasRoomsUpdated = true;
}

bool
USagaNetworkSubSystem::FindRoom(int32 id, FSagaVirtualRoom& outpin)
const noexcept
{
	auto handle = everyRooms.FindByPredicate(FSagaSessionIdComparator{ id });
	if (nullptr != handle)
	{
		outpin = *handle;
		return true;
	}
	else
	{
		return false;
	}
}

bool
USagaNetworkSubSystem::RoomAt(int32 index, FSagaVirtualRoom& outpin)
noexcept
{
	if (everyRooms.IsValidIndex(index))
	{
		outpin = everyRooms[index];
		return true;
	}
	else
	{
		return false;
	}
}

bool
USagaNetworkSubSystem::RemoveRoom(int32 id)
noexcept
{
	const bool result = 0 < everyRooms.RemoveAllSwap(FSagaSessionIdComparator{ id });
	if (result)
	{
		wasRoomsUpdated = true;
	}

	return result;
}

void
USagaNetworkSubSystem::ClearRoomList()
noexcept
{
	everyRooms.Reset();
	wasRoomsUpdated = true;
}

bool
USagaNetworkSubSystem::HasUser(int32 id)
const noexcept
{
	return everyUsers.ContainsByPredicate(FSagaSessionIdComparator{ id });
}

bool
USagaNetworkSubSystem::HasRoom(int32 id)
const noexcept
{
	return everyRooms.ContainsByPredicate(FSagaSessionIdComparator{ id });
}

USagaNetworkSubSystem*
USagaNetworkSubSystem::GetSubSystem(const UWorld* world) noexcept
{
	auto singleton = world->GetGameInstance();

	return singleton->GetSubsystem<USagaNetworkSubSystem>();
}

const TArray<FSagaVirtualUser>&
USagaNetworkSubSystem::GetUserList()
const noexcept
{
	return everyUsers;
}

const TArray<FSagaVirtualRoom>&
USagaNetworkSubSystem::GetRoomList()
const noexcept
{
	return everyRooms;
}

bool
USagaNetworkSubSystem::ConnectToServer(const FString& nickname)
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

AActor*
USagaNetworkSubSystem::CreatePlayableCharacter(UClass* type, const FTransform& transform)
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
