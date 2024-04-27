module;
#include <string_view>

#define SEND(user_var, method, ...) \
auto [io, ctx] = ((user_var).method)(__VA_ARGS__); \
if (not io) \
{ \
	ctx.Complete(); \
}
#define SEND_RESULT(captures, user_var, method, ...) \
[captures](){ \
	auto [io, ctx] = ((user_var).method)(__VA_ARGS__); \
	if (not io) \
	{ \
		ctx.Complete(); \
		return false; \
	} \
	return true; \
}()

module Demo.Framework;
import Iconer.Application.Room;
import Iconer.Application.Rpc;

using namespace iconer::app;

bool
demo::Framework::OnCreatingCharacters(Room& room)
{
	auto number = room.GetMembersCount();
	if (number == 0)
	{
		return false;
	}

	auto& cnt_ref = room.proceedMemberCount;

	room.ForEach
	(
		[&](User& member)
		{
			auto [io, ctx] = member.SendCreateCharactersPacket();
			if (not io)
			{
				ctx.Complete();
			}

			cnt_ref.FetchAdd(1);
		}
	);

#if false
	if (cnt == 0)
	{
		return false;
	}
#else
	if (0 < cnt_ref)
	{
		return true;
	}
#endif

	return false;
}

void
demo::Framework::OnFailedToCreateCharacters(Room& room)
noexcept
{

}

bool
demo::Framework::OnRpc(IContext* ctx, const IdType& user_id)
{
	auto rpc_ctx = std::launder(static_cast<RpcContext*>(ctx));
	if (nullptr == rpc_ctx)
	{
		return false;
	}

	const IdType room_id = rpc_ctx->roomId;
	if (room_id <= -1)
	{
		return false;
	}

	auto room = FindRoom(room_id);
	if (nullptr == room)
	{
		return false;
	}

	if (room->GetMembersCount() == 0)
	{
		return false;
	}

	using enum RpcProtocol;

	switch (rpc_ctx->rpcCategory)
	{
	case RPC_BEG_RIDE:
	{

	}
	break;

	case RPC_END_RIDE:
	{

	}
	break;

	case RPC_BEG_ATTACK_0:
	{

	}
	break;

	case RPC_END_ATTACK_0:
	{

	}
	break;

	default:
	{
		room->ForEach
		(
			[&](User& member)
			{
				SEND(member, SendRpcPacket, user_id, rpc_ctx->rpcCategory, rpc_ctx->firstArgument, rpc_ctx->secondArgument);
			}
		);
	}
	break;
	}

	rpc_ctx->lastOperation = AsyncOperations::OpCleanRpc;
	(void)Schedule(rpc_ctx, 0);

	return true;
}

void
demo::Framework::OnSentRpc(iconer::app::IContext* ctx)
{
	auto rpc = std::launder(static_cast<RpcContext*>(ctx));
	delete rpc;
}

void
demo::Framework::OnCleanRpc(iconer::app::IContext* ctx)
{
	// returns the rpc context object
	auto rpc_ctx = std::launder(static_cast<RpcContext*>(ctx));
	rpc_ctx->lastOperation = AsyncOperations::OpRpc;

	auto it = lastRpcIterator;
	while (true)
	{
		for (; it != everyRpcContexts.end(); ++it)
		{
			auto& stored_ctx = *it;

			RpcContext* null = nullptr;
			if (stored_ctx.compare_exchange_strong(null, rpc_ctx))
			{
				// try to update the last iterator
				bool falsy = false;
				if (lastRpcLock.compare_exchange_strong(falsy, true))
				{
					lastRpcIterator = std::move(it);

					bool truthy = true;
					while (not lastRpcLock.compare_exchange_strong(truthy, false));
				}

				return;
			}
		}

		it = everyRpcContexts.begin();
	}
}
