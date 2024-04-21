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

void
ISagaNetworkRpcView::BeginRun()
{
	auto system = GEngine->GetEngineSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_RUN);
	}
}

void
ISagaNetworkRpcView::EndRun()
{
	auto system = GEngine->GetEngineSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_RUN);
	}
}

void
ISagaNetworkRpcView::BeginJump()
{
	auto system = GEngine->GetEngineSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_JUMP);
	}
}

void
ISagaNetworkRpcView::EndJump()
{
	auto system = GEngine->GetEngineSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_JUMP);
	}
}

void
ISagaNetworkRpcView::BeginAttack()
{
	auto system = GEngine->GetEngineSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_ATTACK_0);
	}
}

void
ISagaNetworkRpcView::EndAttack()
{
	auto system = GEngine->GetEngineSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_ATTACK_0);
	}
}

void
ISagaNetworkRpcView::BeginRide()
{
	auto system = GEngine->GetEngineSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_RIDE);
	}
}

void
ISagaNetworkRpcView::EndRide()
{
	auto system = GEngine->GetEngineSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_RIDE);
	}
}
