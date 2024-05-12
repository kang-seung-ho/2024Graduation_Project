#include "Saga/Network/SagaNetworkSubSystem.h"

void
USagaNetworkSubSystem::OnRpc_Implementation(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1)
{
	bool is_remote{};
	if (id == GetLocalUserId()) // 로컬 클라이언트
	{
#pragma region RPC from Local Client

#pragma endregion
	}
	else // 원격 클라이언트
	{
#pragma region RPC from Remote Client
		is_remote = true;
#pragma endregion
	}

	switch (cat)
	{
	case ESagaRpcProtocol::RPC_POSITION:
	{
		if (is_remote and not IsValid(GetCharacterHandle(id)))
		{
			UE_LOG(LogSagaNetwork, Log, TEXT("[RPC][POSITION] Client %d's position is saved."), id);

			float x{};
			float y{};
			float z{};

			std::memcpy(&x, &arg0, 4);
			std::memcpy(&y, reinterpret_cast<char*>(&arg0) + 4, 4);
			std::memcpy(&z, &arg1, 4);

			StorePosition(id, x, y, z);
		}
	}
	break;

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
