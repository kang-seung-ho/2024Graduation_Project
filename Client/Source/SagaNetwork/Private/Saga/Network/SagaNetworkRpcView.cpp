#include "Saga/Network/SagaNetworkRpcView.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

void
ISagaNetworkRpcView::BeginWalk_Implementation()
{
	auto system = GEngine->GetEngineSubsystem<USagaNetworkSubSystem>();

	if (GetID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK);
	}
}

void
ISagaNetworkRpcView::EndWalk_Implementation()
{
	auto system = GEngine->GetEngineSubsystem<USagaNetworkSubSystem>();

	if (GetID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_WALK);
	}
}

int32
ISagaNetworkRpcView::GetID_Implementation()
const noexcept
{
	return -1;
}
