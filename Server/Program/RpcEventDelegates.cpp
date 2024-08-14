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
ServerFramework::RpcEventOnTryingtoRideGuardian(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	const auto room_id = room.GetID();
	const auto user_id = user.GetID();

	PrintLn("[RPC_BEG_RIDE] At room {}.", room_id);

	if (arg1 < 0 or 3 <= arg1)
	{
		PrintLn("[RPC_BEG_RIDE] User {} tells wrong Guardian {}.", user_id, arg1);

		return;
	}

	auto& guardian = room.sagaGuardians[arg1];

	room.ProcessMember(&user, [&](iconer::app::SagaPlayer& target)
		{
			auto& rider = target.ridingGuardianId;

			const auto rider_id = rider.load(std::memory_order_acquire);
			const auto hp = target.myHp.load(std::memory_order_acquire);

			if (0 < hp and guardian.IsAlive() and guardian.TryRide(user_id))
			{
				std::int32_t pre_guardian_id = -1;

				if (rider.compare_exchange_strong(pre_guardian_id, arg1, std::memory_order_release))
				{
					auto& user_ctx = user.mainContext;
					auto& pk = user_ctx->GetRideGuardianPacketData(arg1);

					room.Foreach
					(
						[&](iconer::app::User& member)
						{
							const auto ctx = AcquireSendContext();

							member.SendGeneralData(*ctx, pk);
						}
					);

					PrintLn("User {} would ride the Guardian {}.", user_id, arg1);
				}
				else
				{
					// rollback
					PrintLn("User {} cannot ride the Guardian {}.", user_id, arg1);

					guardian.TryUnride(user_id);
				}
			}
			else
			{
				PrintLn("User {0} cannot ride the Guardian {1} because guardian {1} is dead.", user_id, arg1);

				std::int32_t pre_guardian_id = arg1;

				// rollback
				rider.compare_exchange_strong(pre_guardian_id, -1, std::memory_order_release);
			}
		}
	);
}

void
ServerFramework::RpcEventOnUnrideFromGuardian(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	const auto room_id = room.GetID();
	const auto user_id = user.GetID();

	PrintLn("[RPC_END_RIDE] At room {}.", room_id);

	if (arg1 < 0 or 3 <= arg1)
	{
		PrintLn("[RPC_END_RIDE] User {} tells wrong guardian {}.", user_id, arg1);

		return;
	}

	auto& guardian = room.sagaGuardians[arg1];

	room.ProcessMember(&user, [&](iconer::app::SagaPlayer& target)
		{
			auto& rider = target.ridingGuardianId;

			const auto rider_id = rider.load(std::memory_order_acquire);
			std::int32_t pre_guardian_id = arg1;

			if (guardian.TryUnride(user_id))
			{
				RpcEventDefault(room, user, RPC_END_RIDE, arg0, arg1);

				rider.compare_exchange_strong(pre_guardian_id, -1, std::memory_order_release);

				PrintLn("[RPC_END_RIDE] User {} would unride from guardian {}.", user_id, arg1);
			}
			else
			{
				std::int32_t pre_guardian_id = arg1;
				rider.compare_exchange_strong(pre_guardian_id, -1, std::memory_order_release);

				PrintLn("[RPC_END_RIDE] User {} was not riding guardian {}.", user_id, arg1);
			}
		}
	);
}

void
ServerFramework::RpcEventOnDamageToGuardian(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	const auto room_id = room.GetID();
	const auto user_id = user.GetID();

	// arg1: index of the guardian
	if (arg1 < 0 or 3 <= arg1)
	{
		PrintLn("User {} tells wrong damaged guardian {}.", user_id, arg1);
		return;
	}

	// arg0: damages to the guardian (4 bytes)
	float dmg{};
	std::memcpy(&dmg, reinterpret_cast<const char*>(&arg0), 4);
	// arg0: destructed body parts (4 bytes, 1 byte per part)
	std::int32_t parts{};
	std::memcpy(&parts, reinterpret_cast<const char*>(&arg0) + 4, 4);

	auto& guardian = room.sagaGuardians[arg1];
	auto& guardian_hp = guardian.myHp;

	auto guardian_hp_value = guardian_hp.load(std::memory_order_acquire);

	if (0 < guardian_hp_value)
	{
		PrintLn("[RPC_DMG_GUARDIAN] At room {} - {} dmg to guardian {}.", room_id, dmg, arg1);

		// 탑승했던 곰 사망
		if (guardian_hp.fetch_sub(dmg, std::memory_order_acq_rel) - dmg <= 0)
		{
			PrintLn("[RPC_DMG_GUARDIAN] At room {} - The guardian {} is dead.", room_id, arg1);

			guardian.myStatus = iconer::app::SagaGuardianState::Dead;

			//std::int64_t hp_arg0{};
			//float curr_hp = guardian_hp.load(std::memory_order_acquire);
			//std::memcpy(&dmg, reinterpret_cast<const char*>(&curr_hp), 4);
			//auto [pk2, size2] = MakeSharedRpc(RPC_DMG_GUARDIAN, user_id, hp_arg0, arg1);

			auto rider_id = guardian.GetRiderId();

			if (-1 != rider_id)
			{
				// 하차 처리
				guardian.TryUnride(rider_id);

				constexpr std::int32_t killIncrement = 3;

				room.Foreach([&](iconer::app::SagaPlayer& member)
					{
						const auto mem_ptr = member.GetStoredUser();
						if (nullptr == mem_ptr) return;

						const auto mem_id = mem_ptr->GetID();
						const auto rider_id_ext = static_cast<std::uint64_t>(rider_id);

						if (mem_id == rider_id_ext)
						{
							auto& team_id = member.myTeamId;
							const auto team = team_id.load(std::memory_order_acquire);

							if (team == ESagaPlayerTeam::Red)
							{
								room.sagaTeamScores[1].fetch_add(killIncrement, std::memory_order_acq_rel);
							}
							else if (team == ESagaPlayerTeam::Blu)
							{
								room.sagaTeamScores[0].fetch_add(killIncrement, std::memory_order_acq_rel);
							}
						}
					}
				);

				auto [pk, size] = MakeSharedRpc(RPC_END_RIDE, rider_id, 0, arg1);

				room.Foreach
				(
					[&](iconer::app::User& member)
					{
						iconer::app::SendContext* const ctx = AcquireSendContext();

						ctx->mySharedBuffer = pk;

						member.SendGeneralData(*ctx, pk.get(), size);
					}
				);

				RpcEventDefault(room, user, RPC_DMG_GUARDIAN, arg0, arg1);
			}
			else // IF (rider id is -1)
			{
				// Send damage first
				RpcEventDefault(room, user, RPC_DMG_GUARDIAN, arg0, arg1);

				constexpr std::int32_t killIncrement = 1;

				room.ProcessMember(&user
					, [&](iconer::app::SagaPlayer& member) noexcept
					{
						// NOTICE: 자기 팀의 점수 증가
						auto& team_id = member.myTeamId;
						const auto team = team_id.load(std::memory_order_acquire);

						if (team == ESagaPlayerTeam::Red)
						{
							room.sagaTeamScores[0].fetch_add(killIncrement, std::memory_order_acq_rel);
						}
						else if (team == ESagaPlayerTeam::Blu)
						{
							room.sagaTeamScores[1].fetch_add(killIncrement, std::memory_order_acq_rel);
						}
					}
				);
			}
		}
		else
		{
			RpcEventDefault(room, user, RPC_DMG_GUARDIAN, arg0, arg1);
		}
	}
	else // IF (guardian hp <= 0)
	{
		PrintLn("[RPC_DMG_GUARDIAN] At room {} - The guardian {} is already dead.", room_id, arg1);
	}
}

void
ServerFramework::RpcEventOnGuardianPartDestructed(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	// arg1: index of the guardian
	if (arg1 < 0 or 3 <= arg1)
	{
		PrintLn("[RPC_DMG_GUARDIANS_PART] User {} tells wrong damaged guardian {}.", user.GetID(), arg1);

		return;
	}

	const auto room_id = room.GetID();
	const auto user_id = user.GetID();

	PrintLn("[RPC_DMG_GUARDIANS_PART] {}(th) part of {} at room {}.", arg0, arg1, room_id);

	auto& guardian = room.sagaGuardians[arg1];
	auto& guardian_hp = guardian.myHp;
	auto& guardian_pt = guardian.myPartHealthPoints;
	auto rider_id = guardian.GetRiderId();

	// 곰 신체부위
	// 0: 파괴 X. 일반적인 경우 송수신 안함. 디버그 용으로 남김
	// 1(0): 오른쪽 팔
	// 2(1): 오른쪽 다리
	// 3(2): 왼쪽 팔
	// 4(3): 왼쪽 다리
	guardian_pt[arg0 - 1] = 0;
	RpcEventDefault(room, user, RPC_DMG_GUARDIANS_PART, arg0 - 1, arg1);

	if (guardian_pt[1] == 0 and guardian_pt[3] == 0)
	{
		PrintLn("[RPC_DMG_GUARDIANS_PART] Guardian {} at room {} is dead.", arg1, room_id);

		RpcEventOnDamageToGuardian(room, user, RPC_DMG_GUARDIAN, 9999, arg1);
	}
}

void
ServerFramework::RpcEventOnRespawn(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	auto& revive_user_ctx = user.mainContext;
	auto& revive_user_pk = revive_user_ctx->GetRespawnPacketData();

	room.ProcessMember(&user, [&](iconer::app::SagaPlayer& target)
		{
			auto& hp = target.myHp;
			hp = iconer::app::SagaPlayer::maxHp;

			room.Foreach
			(
				[&](iconer::app::User& member)
				{
					member.SendGeneralData(*AcquireSendContext(), revive_user_pk);
				}
			);
		}
	);
}

void
ServerFramework::RpcEventOnGettingWeaponChoiceTime(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	const auto now = std::chrono::system_clock::now();
	const auto gap = room.selectionPhaseTime - now;

	user.SendGeneralData(*AcquireSendContext(), room.MakeWeaponChoiceTimerPacket(gap.count()));
}

void
ServerFramework::RpcEventOnGettingRespawnTime(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	room.ProcessMember(&user, [&](iconer::app::SagaPlayer& target)
		{
			const auto now = std::chrono::system_clock::now();

			const auto gap = target.respawnTime - now;
			const auto cnt = gap.count();

			PrintLn("[RPC_RESPAWN_TIMER] User {}'s respawn time: {}.", user.GetID(), cnt);

			if (0 < cnt)
			{
				const auto ctx = AcquireSendContext();

				auto& mem_ctx = user.mainContext;

				user.SendGeneralData(*ctx, mem_ctx->GetRespawnTimePacketData(cnt));
			}
			else
			{
				RpcEventOnRespawn(room, user, RPC_RESPAWN, arg0, arg1);
			}
		}
	);
}

void
ServerFramework::RpcEventOnGettingScores(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	const auto redscore = room.sagaTeamScores[0].load(std::memory_order_acquire);
	const auto bluscore = room.sagaTeamScores[1].load(std::memory_order_acquire);

	user.SendGeneralData(*AcquireSendContext(), room.MakeGameScorePacket(redscore, bluscore));
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

	RpcEventOnGettingScores(room, user, proc, 0, 0);

	if (0 < cnt)
	{
		user.SendGeneralData(*AcquireSendContext(), room.MakeGameTimerPacket(cnt));
	}
	else
	{
		// TODO: 게임 승패 판정 송신
		user.SendGeneralData(*AcquireSendContext(), room.MakeGameTimerPacket(cnt));
	}
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
