export module Iconer.Application.Rpc;
import <cstddef>;
import <cstdint>;
import <type_traits>;
import Iconer.Application.IContext;

#define ICONER_RPC_ENUM_ITEM(name) RPC_BEG_##name, RPC_END_##name

export namespace iconer::app::inline rpc
{
	enum class [[nodiscard]] RpcProtocol : std::uint8_t
	{
		RPC_UNKNOWN = 0,
		ICONER_RPC_ENUM_ITEM(WALK),
		ICONER_RPC_ENUM_ITEM(RUN),
		ICONER_RPC_ENUM_ITEM(JUMP),
		ICONER_RPC_ENUM_ITEM(ATTACK_0), // Normal attack #1
		ICONER_RPC_ENUM_ITEM(ATTACK_1), // Normal attack #2
		ICONER_RPC_ENUM_ITEM(ATTACK_2), // Normal attack #3
		ICONER_RPC_ENUM_ITEM(ATTACK_3), // Normal attack #4
		ICONER_RPC_ENUM_ITEM(RIDE),
		RPC_SKILL_0 = 100, // Ability #1
		RPC_SKILL_1, // Ability #2
		RPC_SKILL_2, // Ability #3
		RPC_SKILL_3, // Ability #4
		RPC_USE_ITEM_0,
		RPC_USE_ITEM_1,
		RPC_USE_ITEM_2,
		RPC_USE_ITEM_3,
		RPC_USE_ITEM_4,
		RPC_SPAWN_ITEM,
		RPC_GRAB_ITEM,
		RPC_MAIN_WEAPON,
		RPC_CHANGE_HAND_ITEM,
		RPC_DMG_PLYER,
		RPC_DMG_GUARDIAN,
		RPC_DMG_GUARDIANS_PART,
		RPC_DEAD,
	};

	class RpcContext : public IContext
	{
	public:
		using Super = IContext;

		constexpr RpcContext(RpcProtocol category, std::int32_t room_id = -1, std::int64_t arg0 = 0, std::int32_t arg1 = 0) noexcept
			: Super(AsyncOperations::OpRpc)
			, rpcCategory(std::move(category))
			, roomId(std::move(room_id))
			, firstArgument(std::move(arg0)), secondArgument(std::move(arg1))
		{}

		RpcProtocol rpcCategory;
		std::int32_t roomId;
		std::int64_t firstArgument;
		std::int32_t secondArgument;
	};
}
