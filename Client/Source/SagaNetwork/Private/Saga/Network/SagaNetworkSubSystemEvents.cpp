#include "Saga/Network/SagaNetworkSubSystem.h"
#include <Sockets.h>

#include "Saga/Network/SagaNetworkWorker.h"
#include "Saga/Network/SagaTaskWorker.h"

#include "Character/SagaCharacterPlayer.h"
#include "Player/SagaUserTeam.h"

void
USagaNetworkSubSystem::OnNetworkInitialized_Implementation(bool succeed)
{
	if (succeed)
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("The network system is initialized."));
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("Cannot initialize the network system."));
	}
}

void
USagaNetworkSubSystem::OnConnected_Implementation()
{
	UE_LOG(LogSagaNetwork, Log, TEXT("Starting workers of network subsystem..."));

	if (nullptr == (netWorker = new FSagaNetworkWorker{ this }))
	{
		UE_LOG(LogSagaNetwork, Fatal, TEXT("Has failed to create the network worker."));
		return;
	}
	else if (nullptr == (taskWorker = new FSagaTaskWorker{ this }))
	{
		UE_LOG(LogSagaNetwork, Fatal, TEXT("Has failed to create the task worker."));
		return;
	}
	else
	{
	}
}

void
USagaNetworkSubSystem::OnFailedToConnect_Implementation(ESagaConnectionContract reason)
{
	const auto msg = UEnum::GetValueAsString(reason);
	UE_LOG(LogSagaNetwork, Error, TEXT("Local client could not connect to the server, due to `%s`"), *msg);
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
USagaNetworkSubSystem::OnFailedToStartGame_Implementation(ESagaGameContract reason)
{
	auto name = UEnum::GetValueAsString(reason);

	UE_LOG(LogSagaNetwork, Error, TEXT("Cannot start the game, due to '%s'."), *name);
}
