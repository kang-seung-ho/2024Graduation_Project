#include "Saga/Network/SagaNetworkRpcView.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

void
ISagaNetworkRpcView::BeginWalk()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK);
	}
}

void
ISagaNetworkRpcView::EndWalk()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_WALK);
	}
}

void
ISagaNetworkRpcView::BeginRun()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_RUN);
	}
}

void
ISagaNetworkRpcView::EndRun()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_RUN);
	}
}

void
ISagaNetworkRpcView::BeginJump()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_JUMP);
	}
}

void
ISagaNetworkRpcView::EndJump()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_JUMP);
	}
}

void
ISagaNetworkRpcView::BeginAttack()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_ATTACK_0);
	}
}

void
ISagaNetworkRpcView::EndAttack()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_ATTACK_0);
	}
}

void
ISagaNetworkRpcView::BeginRide()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_RIDE);
	}
}

void
ISagaNetworkRpcView::EndRide()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (GetRpcID() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_RIDE);
	}
}
