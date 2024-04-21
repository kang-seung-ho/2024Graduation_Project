#include "Saga/Network/SagaNetworkRpcView.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

void
ISagaNetworkRpcView::BeginWalk()
{
	auto system = GEngine->GetEngineSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK);
	}
}

void
ISagaNetworkRpcView::EndWalk()
{
	auto system = GEngine->GetEngineSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_WALK);
	}
}
