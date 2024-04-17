export module Iconer.Application.Rpc;
import <cstddef>;
import <cstdint>;
import <memory>;
import Iconer.Application.IContext;

export namespace iconer::app::inline rpc
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

	class RpcContext : public IContext
	{
	public:
		using Super = IContext;

		constexpr RpcContext(RpcProtocol category, std::int32_t room_id = -1, std::int64_t arg0 = 0, std::int32_t arg1 = 0) noexcept
			: Super(AsyncOperations::OpRpc)
			, rpcCategory(std::move(category))
			, roomId(std::move(room_id))
			, firstArgument(std::move(arg0)), secondArgument(std::move(arg1))
			, rpcBuffer()
		{}

		RpcProtocol rpcCategory;
		std::int32_t roomId;
		std::int64_t firstArgument;
		std::int32_t secondArgument;
		std::unique_ptr<std::byte[]> rpcBuffer;
	};
}
