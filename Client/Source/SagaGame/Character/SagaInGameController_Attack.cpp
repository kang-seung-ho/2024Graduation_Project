#include "SagaInGamePlayerController.h"

#include "SagaCharacterPlayer.h"
#include "Saga/Network/SagaNetworkSettings.h"
#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Saga/Network/SagaRpcProtocol.h"

void
ASagaInGamePlayerController::BeginAttack()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_ATTACK_0);
	}
}

void
ASagaInGamePlayerController::EndAttack()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_ATTACK_0);
	}
}
