#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Sockets.h"
#include "GameFramework/PlayerController.h"

#include "Character/SagaCharacterPlayer.h"
#include "Player/SagaUserTeam.h"

void
USagaNetworkSubSystem::OnNetworkInitialized_Implementation(bool succeed)
{
	if (succeed)
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("The network subsystem is initialized."));
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("Cannot initialize the network subsystem."));
	}
}

void
USagaNetworkSubSystem::OnConnected_Implementation()
{}

void
USagaNetworkSubSystem::OnFailedToConnect_Implementation(ESagaConnectionContract reason)
{
	auto msg = UEnum::GetValueAsString(reason);
	UE_LOG(LogSagaNetwork, Log, TEXT("Local client can't get an id from server due to %s."), *msg);
}

void
USagaNetworkSubSystem::OnDisconnected_Implementation()
{
	if (IsSocketAvailable())
	{
		(void)CloseNetwork_Implementation();
	}
}

void
USagaNetworkSubSystem::OnRoomCreated_Implementation(int32 id)
{
	UE_LOG(LogSagaNetwork, Log, TEXT("A room %d is created."), id);

	SendRequestMembersPacket();
}

void
USagaNetworkSubSystem::OnJoinedRoom_Implementation(int32 id)
{}

void
USagaNetworkSubSystem::OnLeftRoomBySelf_Implementation()
{
	UE_LOG(LogSagaNetwork, Log, TEXT("Local client has been left from room."));
}

void
USagaNetworkSubSystem::OnLeftRoom_Implementation(int32 id)
{
	UE_LOG(LogSagaNetwork, Log, TEXT("Remote client %d has been left from room."), id);
}

void
USagaNetworkSubSystem::OnRespondVersion_Implementation(const FString& version_string)
{
	UE_LOG(LogSagaNetwork, Log, TEXT("Version: %s"), *version_string);
}

void
USagaNetworkSubSystem::OnUpdateRoomList_Implementation(const TArray<FSagaVirtualRoom>& list)
{}

void
USagaNetworkSubSystem::OnUpdateMembers_Implementation(const TArray<FSagaVirtualUser>& list)
{}

void
USagaNetworkSubSystem::OnTeamChanged_Implementation(int32 user_id, bool is_red_team)
{
	for (auto& user : everyUsers)
	{
		if (user.MyID == user_id)
		{
			user.myTeam = is_red_team ? EUserTeam::Red : EUserTeam::Blue;
		}
	}
}

void
USagaNetworkSubSystem::OnGetPreparedGame_Implementation()
{}

void
USagaNetworkSubSystem::OnFailedToStartGame_Implementation(ESagaGameContract reason)
{
	auto name = UEnum::GetValueAsString(reason);

	UE_LOG(LogSagaNetwork, Error, TEXT("Cannot start the game, due to '%s'."), *name);
}

void
USagaNetworkSubSystem::OnStartGame_Implementation()
{
	const auto player = GEngine->FindFirstLocalPlayerFromControllerId(0);
	if (nullptr == player)
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][OnStartGame] Cannot find a handle of the local player."));
		return;
	}

	const auto world = player->GetWorld();
	if (nullptr == world)
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][OnStartGame] The handle of world is null."));
		return;
	}

	auto controller = player->GetPlayerController(world);
	if (nullptr == controller)
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][OnStartGame] Cannot find the local player's controller."));
		return;
	}

	if (nullptr == controller->GetPawn())
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][OnStartGame] Cannot find a pawn of the local player."));
		return;
	}

	UE_LOG(LogSagaNetwork, Log, TEXT("[SagaGame][OnStartGame] System found the character of local player."));
}

void
USagaNetworkSubSystem::OnUpdatePosition_Implementation(int32 id, float x, float y, float z)
{
	if (id == GetLocalUserId()) // 로컬 클라이언트
	{

	}
	else // 원격 클라이언트
	{
		FSagaVirtualUser user{};

		if (not FindUser(id, user))
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][OnUpdatePosition] Cannot find remote player %d."), id);
			return;
		}

		auto& character = user.remoteCharacter;
		if (not IsValid(character))
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][OnUpdatePosition] Cannot find a character of remote player %d'."), id);
			return;
		}

		character->SetActorLocation(FVector{ x, y, z });
	}
}

void
USagaNetworkSubSystem::OnUpdateRotation_Implementation(int32 id, float p, float y, float r)
{
	if (id == GetLocalUserId()) // 로컬 클라이언트
	{
	}
	else // 원격 클라이언트
	{
		FSagaVirtualUser user{};

		if (not FindUser(id, user))
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][OnUpdateRotation] Cannot find remote player %d."), id);
			return;
		}

		auto& character = user.remoteCharacter;
		if (not IsValid(character))
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][OnUpdateRotation] Cannot find a character of remote player %d'."), id);
			return;
		}

		character->SetActorRotation(FRotator{ p, y, r });
	}
}
