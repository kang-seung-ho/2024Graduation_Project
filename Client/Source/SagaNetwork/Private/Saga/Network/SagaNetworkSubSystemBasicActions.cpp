#include "Saga/Network/SagaNetworkSubSystem.h"
#include <UObject/Class.h>
#include <Internationalization/Text.h>
#include <Containers/UnrealString.h>

#include "Saga/Network/SagaNetworkWorker.h"
#include "Saga/Network/SagaTaskWorker.h"

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
		if (not IsOfflineMode())
		{
			UE_LOG(LogSagaNetwork, Fatal, TEXT("Has failed to create the network worker."));
		}
		else
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("Has failed to create the network worker. (Offline Mode)"));
		}
	}
	else if (nullptr == (taskWorker = new FSagaTaskWorker{ this }))
	{
		if (not IsOfflineMode())
		{
			UE_LOG(LogSagaNetwork, Fatal, TEXT("Has failed to create the task worker."));
		}
		else
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("Has failed to create the task worker. (Offline Mode)"));
		}
	}
}

void
USagaNetworkSubSystem::OnFailedToConnect_Implementation(ESagaConnectionContract reason)
{
	const FString desc = UEnum::GetValueAsString(reason);

	UE_LOG(LogSagaNetwork, Error, TEXT("Local client could not connect to the server, due to `%s`"), *desc);
}

void
USagaNetworkSubSystem::OnDisconnected_Implementation()
{
	if (IsSocketAvailable())
	{
		(void)CloseNetwork_Implementation();
	}

	ClearUserList(true);
	ClearRoomList();

	SetLocalUserId(-1);
	SetLocalUserName(FText::GetEmpty());
	SetLocalUserTeam(ESagaPlayerTeam::Unknown);
	SetCurrentRoomId(-1);
	currentRoom.MembersCount = 0;
}

void
USagaNetworkSubSystem::OnFailedToStartGame_Implementation(ESagaGameContract reason)
{
	const FString desc = UEnum::GetValueAsString(reason);

	UE_LOG(LogSagaNetwork, Error, TEXT("Cannot start the game, due to '%s'."), *desc);
}
