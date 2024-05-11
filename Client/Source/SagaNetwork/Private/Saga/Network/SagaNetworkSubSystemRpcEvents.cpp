#include "Saga/Network/SagaNetworkSubSystem.h"

void
USagaNetworkSubSystem::OnRpc_Implementation(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1)
{
	if (id == GetLocalUserId()) // 로컬 클라이언트
	{
#pragma region RPC from Local Client

#pragma endregion
	}
	else // 원격 클라이언트
	{
#pragma region RPC from Remote Client

#pragma endregion
	}

	switch (cat)
	{
	case ESagaRpcProtocol::RPC_MAIN_WEAPON:
	{
		const auto new_weapon = static_cast<EPlayerWeapon>(arg0);
		SetWeapon(id, new_weapon);

		EPlayerWeapon test{};
		GetWeapon(id, test);
		ensure(new_weapon == test);

		UE_LOG(LogSagaNetwork, Log, TEXT("[RPC] Client %d's weapon is %d"), id, static_cast<int>(test));
	}
	break;

	default:
		break;
	}
}
