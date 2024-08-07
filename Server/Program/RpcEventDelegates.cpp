module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]
#include <chrono>
#include <atomic>

module Iconer.Framework;
import Iconer.App.SendContext;
import Iconer.App.PacketSerializer;
import Iconer.App.RpcProtocols;
import Iconer.App.GameSession;

using iconer::app::RpcProtocol;
using iconer::app::ESagaPlayerTeam;
using enum iconer::app::RpcProtocol;

namespace
{
	inline constexpr std::chrono::seconds gameAwaitPhasePeriod{ 31 };
	inline constexpr std::chrono::seconds respawnPeriod{ 5 };

	[[nodiscard]]
	constexpr std::pair<std::unique_ptr<std::byte[]>, std::int16_t>
		MakeRpc(iconer::app::RpcProtocol proc, const std::int32_t& id, const std::int64_t& arg0, const std::int32_t& arg1)
	{
		return iconer::app::Serialize(iconer::app::PacketProtocol::SC_RPC, id, proc, arg0, arg1);
	}

	[[nodiscard]]
	std::pair<std::shared_ptr<std::byte[]>, std::int16_t>
		MakeSharedRpc(iconer::app::RpcProtocol proc, const std::int32_t& id, const std::int64_t& arg0, const std::int32_t& arg1)
	{
		auto [ptr, size] = iconer::app::Serialize(iconer::app::PacketProtocol::SC_RPC, id, proc, arg0, arg1);

		return std::make_pair(std::shared_ptr<std::byte[]>(ptr.release()), size);
	}
}

void
ServerFramework::RpcEventOnWeaponChanged(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	// arg0: kind of weapon
	// 0: lightsaber
	// 1: watergun
	// 2: hammer
	if (room.ProcessMember(&user, [&](iconer::app::SagaPlayer& member)
		{
			member.myWeapon = static_cast<std::uint8_t>(arg0);
		}
	))
	{
		RpcEventDefault(room, user, proc, arg0, arg1);
	}
}

void
ServerFramework::RpcEventOnItemBoxDestroyed(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	auto& items = room.sagaItemList;

	const auto index = static_cast<size_t>(arg1);
	auto& item = items[index];

	bool destroyed = false;
	if (item.isBoxDestroyed.compare_exchange_strong(destroyed, true))
	{
		PrintLn("[RPC_DESTROY_ITEM_BOX] At room {} - {}.", room.GetID(), arg1);

		RpcEventDefault(room, user, proc, arg0, arg1);
	}
	else
	{
		PrintLn("[RPC_DESTROY_ITEM_BOX] At room {} - {} is already destroyed.", room.GetID(), arg1);
	}
}

void
ServerFramework::RpcEventOnItemGrabbed(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	auto& items = room.sagaItemList;

	if (arg1 < 0 or 200 <= arg1) return;

	const auto index = static_cast<size_t>(arg1);
	auto&& item = items[index];

	if (item.isBoxDestroyed)
	{
		bool grabbed = true;
		if (item.isAvailable.compare_exchange_strong(grabbed, false))
		{
			PrintLn("[RPC_GRAB_ITEM] At room {} - {}.", room.GetID(), arg1);

			RpcEventDefault(room, user, proc, arg0, arg1);
		}
		else
		{
			PrintLn("[RPC_GRAB_ITEM] At room {} - {} is already attained.", room.GetID(), arg1);
		}
	}
	else
	{
		PrintLn("[RPC_GRAB_ITEM] At room {} - {} is not destroyed yet.", room.GetID(), arg1);
	}
}

void
ServerFramework::RpcEventOnItemUsed(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	const auto room_id = room.GetID();
	const auto user_id = user.GetID();

	std::int64_t item_effect_v = arg0;
	std::int64_t item_type = arg1;

	PrintLn("[RPC_USE_ITEM_0] At room {}.", room_id);

	switch (arg1)
	{
		// Energy Drink
	case 0:
	{
		room.ProcessMember(&user, [&](iconer::app::SagaPlayer& target)
			{
				PrintLn("[RPC_USE_ITEM_0] At room {} - Energy drink for user {}.", room_id, user_id);

				auto& rider = target.ridingGuardianId;

				const auto rider_id = rider.load(std::memory_order_acquire);

				if (rider_id == -1)
				{
					auto& hp = target.myHp;

					if (const auto hp_value = hp.load(std::memory_order_acquire); hp_value < target.maxHp)
					{
						hp.store(std::min(iconer::app::SagaPlayer::maxHp, hp_value + 30.0f), std::memory_order_release);
						item_effect_v = 30;
					}
					else
					{
						item_effect_v = 0;
					}
				}
				else
				{
					auto& guardian = room.sagaGuardians[rider_id];
					auto& guardian_hp = guardian.myHp;

					if (const auto hp_value = guardian_hp.load(std::memory_order_acquire); hp_value < guardian.maxHp)
					{
						guardian_hp.store(std::min(iconer::app::SagaGuardian::maxHp, hp_value + 30.0f), std::memory_order_release);
						item_effect_v = 30;
					}
					else
					{
						item_effect_v = 0;
					}
				}
			}
		);
	}
	break;

	case 1:
	{}
	break;

	case 2:
	{}
	break;

	default:
	{}
	break;
	}

	RpcEventDefault(room, user, proc, item_effect_v, item_type);
}

void
ServerFramework::RpcEventOnGettingScores(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	const auto redscore = room.sagaTeamScores[0].load(std::memory_order_acquire);
	const auto bluscore = room.sagaTeamScores[1].load(std::memory_order_acquire);

	iconer::app::SendContext* const ctx = AcquireSendContext();
	auto [pk, size] = MakeRpc(RPC_GET_SCORE, 0, redscore, bluscore);

	ctx->myBuffer = std::move(pk);

	user.SendGeneralData(*ctx, size);
}

void
ServerFramework::RpcEventOnGettingGameTime(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	const auto now = std::chrono::system_clock::now();

	const auto gap = room.gamePhaseTime - now;
	const auto cnt = gap.count();

	if (0 < cnt)
	{
	}
	else
	{
		RpcEventOnGettingScores(room, user, proc, 0, 0);
	}

	user.SendGeneralData(*AcquireSendContext(), room.MakeGameTimerPacket(cnt));
}

void
ServerFramework::RpcEventDefault(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	//PrintLn(L"\tRPC is proceed At room {}.", room_id);

	auto [pk, size] = MakeSharedRpc(proc, user.GetID(), arg0, arg1);

	room.Foreach
	(
		[&](iconer::app::User& member)
		{
			iconer::app::SendContext* const ctx = AcquireSendContext();

			ctx->mySharedBuffer = pk;

			member.SendGeneralData(*ctx, pk.get(), size);
		}
	);
}
