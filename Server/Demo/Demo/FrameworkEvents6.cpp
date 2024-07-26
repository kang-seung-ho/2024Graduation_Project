module;
#include <chrono>
#include <string_view>
#include <print>

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

bool
demo::Framework::OnRpc(IContext* ctx, const IdType& user_id)
{
	auto rpc_ctx = std::launder(static_cast<RpcContext*>(ctx));
	if (nullptr == rpc_ctx)
	{
		delete rpc_ctx;

		return false;
	}

	const IdType& room_id = rpc_ctx->roomId;
	if (room_id <= -1)
	{
		delete rpc_ctx;

		return false;
	}

	const auto room = FindRoom(room_id);
	if (nullptr == room)
	{
		delete rpc_ctx;

		return false;
	}

	if (room->GetMembersCount() == 0)
	{
		delete rpc_ctx;

		return false;
	}

	const auto user = FindUser(user_id);
	if (nullptr == user)
	{
		delete rpc_ctx;

		return false;
	}

	auto& arg0 = rpc_ctx->firstArgument;
	auto& arg1 = rpc_ctx->secondArgument;

	using enum RpcProtocol;

	switch (rpc_ctx->rpcCategory)
	{
	case RPC_ASSIGN_ITEM_ID:
	{
		myLogger.DebugLog(L"\t[RPC_ASSIGN_ITEM_ID] at room {}\n", room_id);

		if (room->IsEmpty())
		{
			break;
		}

		myLogger.DebugLog(L"User {} claims {} item spawners.\n", user_id, arg1);
	}
	break;

	// arg0 - nothing
	// arg1 - id
	case RPC_DESTROY_ITEM_BOX:
	{
		myLogger.DebugLog(L"\t[RPC_DESTROY_ITEM_BOX] at room {}\n", room_id);

		auto& items = room->sagaItemList;
		auto& ilock = room->sagaItemListLock;

		while (true)
		{
			bool flag = false;
			if (ilock.compare_exchange_strong(flag, true)) break;
		}

		if (room->IsEmpty())
		{
			break;
		}

		const auto index = static_cast<size_t>(arg1);
		iconer::app::game::SagaItem& item = items[index];

		bool destroyed = false;
		if (item.isBoxDestroyed.compare_exchange_strong(destroyed, true))
		{
			myLogger.DebugLog(L"\tUser {} destroys item spawner {}.\n", user_id, arg1);

			room->ForEach
			(
				[&](iconer::app::User& member)
				{
					SEND(member, SendRpcPacket, user_id, RPC_DESTROY_ITEM_BOX, arg0, arg1);
				}
			);
		}

		ilock = false;
	}
	break;

	// 
	// arg0 - nothing
	// arg1 - id
	case RPC_GRAB_ITEM:
	{
		myLogger.DebugLog(L"\t[RPC_GRAB_ITEM] at room {} by user {}\n", room_id, user_id);

		auto& items = room->sagaItemList;

		if (room->IsEmpty())
		{
			break;
		};

		///TODO: max item count
		if (arg1 < 0) break;

		const auto index = static_cast<size_t>(arg1);
		iconer::app::game::SagaItem& item = items[index];

		bool grabbed = true;

		if (item.isAvailable.compare_exchange_strong(grabbed, false))
		{
			myLogger.DebugLog(L"\t[RPC_GRAB_ITEM] User {} gets item {}.\n", user_id, arg1);

			SEND(*user, SendRpcPacket, user_id, RPC_GRAB_ITEM, arg0, arg1);
		}
	}
	break;

	// 
	// arg0 - inventory index
	// arg1 - item type
	case RPC_USE_ITEM_0:
	{
		switch (arg1)
		{
		case 0:
		{
			myLogger.DebugLog(L"\t[RPC_USE_ITEM] Energy Drink at room {}\n", room_id);

			user->myHealth.FetchAdd(30);
		}
		break;

		case 1:
		{
			myLogger.DebugLog(L"\t[RPC_USE_ITEM] Gumball at room {}\n", room_id);

			//user->myHealth.FetchAdd(30);
		}
		break;

		case 2:
		{
			myLogger.DebugLog(L"\t[RPC_USE_ITEM] Smoke bomb at room {}\n", room_id);

			//user->myHealth.FetchAdd(30);
		}
		break;

		default:
		{
			myLogger.DebugLogError(L"\t[RPC_USE_ITEM] Unknown item {} at room {}\n", arg1, room_id);
		}
		break;
		}

		room->ForEach
		(
			[&](iconer::app::User& member)
			{
				SEND(member, SendRpcPacket, user_id, RPC_USE_ITEM_0, arg0, arg1);
			}
		);
	}
	break;

	case RPC_MAIN_WEAPON:
	{
		// arg0: kind of weapon
		// 0: lightsaber
		// 1: watergun
		// 2: hammer
		user->myWeaponId.Store(static_cast<std::uint8_t>(arg0));

		// broadcast his weapon
		room->ForEach
		(
			[&](User& member)
			{
				// NOTICE: RPC_MAIN_WEAPON(arg0, 0)
				SEND(member, SendRpcPacket, user_id, rpc_ctx->rpcCategory, arg0, 0);
			}
		);

		myLogger.DebugLog(L"\tUser {} changed weapon to {}\n", user_id, arg0);
	}
	break;

	case RPC_BEG_RIDE:
	{
		myLogger.DebugLog(L"\t[RPC_BEG_RIDE] at room {}\n", room_id);

		// arg1: index of guardian
		if (arg1 < 0 or 3 <= arg1)
		{
			myLogger.LogError(L"\tUser {} tells wrong Guardian {}\n", user_id, arg1);
			break;
		}

		auto& guardian = room->sagaGuardians[arg1];
		auto& riding = user->isRidingGuardian;

		// TODO: RPC_BEG_RIDE

		if (0 < user->myHealth and riding.CompareAndSet(false, true, std::memory_order_acq_rel))
		{
			if (guardian.IsAlive())
			{
				myLogger.DebugLog(L"\tUser {} would ride the Guardian {}.\n", user_id, arg1);

				if (guardian.TryRide(user_id))
				{
					room->ForEach
					(
						[&](User& member)
						{
							SEND(member, SendRpcPacket, user_id, RPC_BEG_RIDE, arg0, arg1);
						}
					);
				}
				else
				{
					myLogger.LogWarning(L"\tGuardian {} is already occupied.\n", arg1);

					riding.CompareAndSet(true, false, std::memory_order_acq_rel);
				}
			}
			else
			{
				myLogger.LogWarning(L"\tGuardian {} is dead so user {} cannot ride it.\n", user_id, arg1);

				riding.CompareAndSet(true, false, std::memory_order_acq_rel);
			}
		}
		else
		{
			myLogger.LogWarning(L"\tUser {} cannot ride the Guardian {}.\n", user_id, arg1);
		}
	}
	break;

	case RPC_END_RIDE:
	{
		myLogger.DebugLog(L"\t[RPC_END_RIDE] at room {}\n", room_id);

		// arg1: index of guardian
		if (arg1 < 0 or 3 <= arg1)
		{
			myLogger.LogError(L"\tUser {} tells wrong guardian {}\n", user_id, arg1);
			break;
		}

		bool is_riding = user->isRidingGuardian.Load(std::memory_order_acquire);

		auto& guardian = room->sagaGuardians[arg1];

		if (guardian.TryUnride(user_id))
		{
			myLogger.DebugLog(L"\tUser {} would unride from guardian {}\n", user_id, arg1);

			room->ForEach
			(
				[&](User& member)
				{
					SEND(member, SendRpcPacket, user_id, RPC_END_RIDE, arg0, arg1);
				}
			);

			user->isRidingGuardian.Store(false, std::memory_order_release);
		}
		else
		{
			myLogger.LogWarning(L"\tUser {} was not riding guardian {}\n", user_id, arg1);

			user->isRidingGuardian.Store(is_riding, std::memory_order_release);
		}
	}
	break;

	case RPC_BEG_ATTACK_0:
	{
		//myLogger.DebugLog(L"\t[RPC_BEG_ATTACK_0] at room {}\n", room_id);

		//if (0 < user->myHealth)
		{
			room->ForEach
			(
				[rpc_ctx, user_id](User& member)
				{
					SEND(member, SendRpcPacket, user_id, rpc_ctx->rpcCategory, rpc_ctx->firstArgument, rpc_ctx->secondArgument);
				}
			);
		}
	}
	break;

	case RPC_DMG_PLYER:
	{
		// arg0: 플레이어가 준 피해량(4바이트 부동소수점) | 플레이어 상태 (곰/인간) (4바이트 정수)
		// arg1: 플레이어에게 피해를 준 개체의 식별자
		//     예시: 플레이어의 ID, 수호자의 순번

		auto& hp = user->myHealth;

		float dmg{};
		std::memcpy(&dmg, reinterpret_cast<const char*>(&arg0), 4);
		myLogger.DebugLog(L"\t[RPC_DMG_PLYER] At room {} - {} dmg from user {}\n", room_id, dmg, arg1);

		if (0 < hp)
		{
			hp.FetchSubtract(dmg);

			if (hp <= 0)
			{
				// 하차 처리
				{
					//room->ForEach
					//(
					//	[&](User& member)
						//{
							//SEND(member, SendRpcPacket, user_id, RPC_END_RIDE, arg0, arg1);
						//}
					//);
				}

				// 사망
				user->isRidingGuardian = false;
				user->respawnTime = std::chrono::system_clock::now() + User::respawnPeriod;

				room->ForEach
				(
					[&](User& member)
					{
						SEND(member, SendRpcPacket, user_id, RPC_DEAD, user_id, 0);
					}
				);
			}
			else
			{
				// 데미지 처리
				room->ForEach
				(
					[&](User& member)
					{
						SEND(member, SendRpcPacket, user_id, RPC_DMG_PLYER, arg0, arg1);
					}
				);
			}
		}
	}
	break;

	case RPC_DMG_GUARDIAN:
	{
		myLogger.DebugLog(L"\t[RPC_DMG_GUARDIAN] at room {}\n", room_id);

		float dmg{};
		std::memcpy(&dmg, reinterpret_cast<const char*>(&arg0), 4);
		myLogger.DebugLog(L"\t[RPC_DMG_GUARDIAN] At room {} - {} dmg to guardian {}\n", room_id, dmg, arg1);

		// 데미지 처리
		room->ForEach
		(
			[&](User& member)
			{
				SEND(member, SendRpcPacket, user_id, RPC_DMG_GUARDIAN, arg0, arg1);
			}
		);
	}
	break;

	// 일반적인 경우 실행안됨
	// 오직 ExecuteRespawnViaRpc 메서드로부터만 수신됨
	case RPC_RESPAWN:
	{
		auto& hp = user->myHealth;
		hp = 100.0f;

		room->ForEach
		(
			[&](User& member)
			{
				SEND(member, SendRpcPacket, user_id, RPC_RESPAWN, arg0, arg1);
			}
		);
	}
	break;

	case RPC_RESPAWN_TIMER:
	{
		const auto now = std::chrono::system_clock::now();

		if (now <= user->respawnTime)
		{
			// 
		}

		const auto gap = user->respawnTime - now;
		const auto cnt = gap.count();
		//myLogger.DebugLog(L"\tUser {}'s respawn time: {}\n", user_id, cnt);

		if (0 < cnt)
		{
			SEND(*user, SendRpcPacket, user_id, RPC_RESPAWN_TIMER, cnt, arg1);
		}
		else
		{
			// RPC_RESPAWN랑 똑같은 처리
			auto& hp = user->myHealth;
			hp = 100.0f;

			room->ForEach
			(
				[&](User& member)
				{
					SEND(member, SendRpcPacket, user_id, RPC_RESPAWN, arg0, arg1);
				}
			);
		}
	}
	break;

	case RPC_DMG_GUARDIANS_PART:
	{
		myLogger.DebugLog(L"\t[RPC_DMG_GUARDIANS_PART] at room {}\n", room_id);

	}
	break;

	case RPC_WEAPON_TIMER:
	{
		const auto now = std::chrono::system_clock::now();

		if (now <= room->selectionPhaseTime)
		{
			// 
		}

		const auto gap = room->selectionPhaseTime - now;
		const auto cnt = gap.count();
		//myLogger.DebugLog(L"\tRoom {}'s weapon phase: {}\n", room_id, cnt);

		SEND(*user, SendRpcPacket, user_id, rpc_ctx->rpcCategory, cnt, arg1);
	}
	break;

	case RPC_GAME_TIMER:
	{
		const auto now = std::chrono::system_clock::now();

		if (now <= room->gamePhaseTime)
		{
			// 
		}

		const auto gap = room->gamePhaseTime - now;
		const auto cnt = gap.count();


		SEND(*user, SendRpcPacket, user_id, rpc_ctx->rpcCategory, cnt, arg1);
	}
	break;

	case RPC_NOTIFY_GAME_COUNTDOWN:
	{
		const auto now = std::chrono::system_clock::now();

		if (now <= room->gamePhaseTime)
		{
			// 
		}

		const auto gap = room->gamePhaseTime - now;
		const auto cnt = gap.count();
		//myLogger.DebugLog(L"\tRoom {}'s game time: {}\n", room_id, cnt);

		SEND(*user, SendRpcPacket, user_id, rpc_ctx->rpcCategory, cnt, arg1);
	}
	break;

	default:
	{
		//myLogger.DebugLog(L"\tRPC is proceed at room {}\n", room_id);

		room->ForEach
		(
			[rpc_ctx, user_id](User& member)
			{
				SEND(member, SendRpcPacket, user_id, rpc_ctx->rpcCategory, rpc_ctx->firstArgument, rpc_ctx->secondArgument);
			}
		);
	}
	break;
	}

	//rpc_ctx->lastOperation = AsyncOperations::OpCleanRpc;
	//(void)Schedule(rpc_ctx, 0);
	delete rpc_ctx;

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

bool
demo::Framework::OnUpdatingRoom(iconer::app::Room& room)
{
	if (room.GetState() == iconer::app::RoomStates::InGame)
	{
		//auto list = room.

		return true;
	}
	else
	{
		return false;
	}
}

void
demo::Framework::OnFailedToUpdateRoom(iconer::app::Room& room)
noexcept
{
	if (room.GetMembersCount() == 0)
	{
		if (room.TryCancelBeginGame())
		{
			room.Cleanup();
		}
	}
	else if (room.GetState() == iconer::app::RoomStates::InGame)
	{

	}
	else
	{

	}
}

void
demo::Framework::OnFailedToCreateCharacters(Room& room)
noexcept
{}
