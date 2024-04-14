export module Iconer.Application.Rpc;
import <cstddef>;
import <cstdint>;
import <memory>;

export namespace iconer::app
{
	enum class [[nodiscard]] RpcProtocol : std::uint8_t
	{
		RPC_UNKNOWN = 0,
		RPC_WALK,
		RPC_RUN,
		RPC_JUMP,
		RPC_RIDE,
		RPC_ATTACK_0, // Normal attack #1
		RPC_ATTACK_1, // Normal attack #2
		RPC_ATTACK_2, // Special Attack #3
		RPC_ATTACK_3, // Special Attack #4
		RPC_SKILL_0, // Ability #1
		RPC_SKILL_1, // Ability #2
		RPC_SKILL_2, // Ability #3
		RPC_SKILL_3, // Ability #4
		RPC_USE_ITEM_0,
		RPC_USE_ITEM_1,
		RPC_USE_ITEM_2,
		RPC_USE_ITEM_3,
		RPC_USE_ITEM_4,
		RPC_CHANGE_HAND_ITEM,
		RPC_DEAD,
	};

	struct [[nodiscard]] SerializedRpcPacket
	{
		std::unique_ptr<std::byte[]> buffer;
	};
}
