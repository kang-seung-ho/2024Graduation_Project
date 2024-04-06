#include "Saga/GameMode/SagaMainMenuMode.h"
#include "Saga/Network/SagaNetworkSystem.h"

bool
ASagaMainMenuMode::TryLoginToServer(FString nickname)
{
	if (saga::USagaNetwork::Start(MoveTemp(nickname)))
	{
		UE_LOG(LogNet, Log, TEXT("The network system is started."));
		return true;
	}
	else
	{
		UE_LOG(LogNet, Error, TEXT("Cannot start the network system."));
		return false;
	}
}
