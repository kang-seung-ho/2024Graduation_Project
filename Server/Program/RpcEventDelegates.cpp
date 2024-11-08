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

	void Broadcast(ServerFramework& framework
		, iconer::app::Room& room
		, iconer::app::RpcProtocol proc
		, const std::int32_t& id
		, const std::int64_t& arg0, const std::int32_t& arg1)
	{
		auto [pk, size] = MakeSharedRpc(proc, id, arg0, arg1);

		room.Foreach
		(
			[&](iconer::app::User& member)
			{
				iconer::app::SendContext* const ctx = framework.AcquireSendContext();

				ctx->mySharedBuffer = pk;

				member.SendGeneralData(*ctx, pk.get(), size);
			}
		);
	};
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
	if (room.IsGameEnded())
	{
		return;
	}

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
	if (room.IsGameEnded())
	{
		return;
	}

	if (arg1 < 0 or 3 <= arg1)
	{
		PrintLn("[RPC_BEG_RIDE] User {} tells wrong Guardian {}.", user.GetID(), arg1);

		return;
	}

	const auto user_id = static_cast<std::int32_t>(user.GetID());

	auto& guardian = room.sagaGuardians[arg1];

	room.ProcessMember(&user, [&](iconer::app::SagaPlayer& target)
		{
			auto& ridden_guardian_id = target.ridingGuardianId;

			if (0 < target.myHp.load(std::memory_order_acquire) and guardian.IsAlive() and guardian.TryRide(user_id))
			{
				std::int32_t pre_guardian_id = -1;

				if (ridden_guardian_id.compare_exchange_strong(pre_guardian_id, arg1, std::memory_order_release))
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
					guardian.TryUnride(user_id);

					PrintLn("User {} cannot ride the Guardian {}.", user_id, arg1);
				}
			}
			else
			{
				PrintLn("User {0} cannot ride the Guardian {1} because guardian {1} is dead.", user_id, arg1);

				std::int32_t pre_guardian_id = arg1;

				// rollback
				ridden_guardian_id.compare_exchange_strong(pre_guardian_id, -1, std::memory_order_release);
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
	if (room.IsGameEnded())
	{
		return;
	}

	if (arg1 < 0 or 3 <= arg1)
	{
		PrintLn("[RPC_END_RIDE] User {} tells wrong guardian {}.", arg0, arg1);

		return;
	}

	auto& guardian = room.sagaGuardians[arg1];
	const auto rider_id = static_cast<std::int32_t>(arg0);

	room.ProcessMember(rider_id, [&](iconer::app::SagaPlayer& rider_session)
		{
			auto& ridden_guardian_id = rider_session.ridingGuardianId;

			std::int32_t pre_guardian_id = arg1;
			ridden_guardian_id.compare_exchange_strong(pre_guardian_id, -1, std::memory_order_acq_rel);
		}
	);

	if (guardian.TryUnride(rider_id))
	{
		Broadcast(*this, room, RPC_END_RIDE, rider_id, rider_id, arg1);

		PrintLn("[RPC_END_RIDE] User {} would unride from guardian {}.", rider_id, arg1);
	}
	else
	{
		PrintLn("[RPC_END_RIDE] User {} was not riding guardian {}.", rider_id, arg1);
	}
}

void
ServerFramework::RpcEventOnDamageToGuardian(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	if (room.IsGameEnded())
	{
		return;
	}

	// arg1: index of the guardian
	if (arg1 < 0 or 3 <= arg1)
	{
		PrintLn("[RPC_DMG_GUARDIAN] User {} tells wrong damaged guardian {}.", user.GetID(), arg1);
		return;
	}

	// arg0: damages to the guardian (4 bytes)
	float dmg{};
	std::memcpy(&dmg, reinterpret_cast<const char*>(&arg0), 4);

	auto& guardian = room.sagaGuardians[arg1];
	auto& guardian_hp = guardian.myHp;

	if (auto guardian_hp_value = guardian_hp.load(std::memory_order_acquire); 0 < guardian_hp_value)
	{
		// 곰 사망
		if (guardian_hp.fetch_sub(dmg, std::memory_order_acq_rel) - dmg <= 0)
		{
			if (auto rider_id = guardian.GetRiderId(); -1 != rider_id)
			{
				room.ProcessMember(rider_id, [&](iconer::app::SagaPlayer& rider_session)
					{
						auto& team_id = rider_session.myTeamId;

						// 탑승자의 상대 팀의 점수 증가
						constexpr std::int32_t kill_increment = 3;
						room.AddOppositeTeamScore(team_id.load(std::memory_order_acquire), kill_increment);
					}
				);

				// 하차 처리
				RpcEventOnUnrideFromGuardian(room, user, RPC_END_RIDE, rider_id, arg1);

				PrintLn("[RPC_DMG_GUARDIAN] At room {} guardian {} with rider {} is dead.", room.GetID(), arg1, rider_id);
			}
			else // IF (rider id is -1)
			{
				room.ProcessMember(&user, [&](iconer::app::SagaPlayer& session) noexcept
					{
						auto& team_id = session.myTeamId;

						// 공격자의 팀의 점수 증가
						constexpr std::int32_t kill_increment = 1;
						room.AddTeamScore(team_id.load(std::memory_order_acquire), kill_increment);
					}
				);

				PrintLn("[RPC_DMG_GUARDIAN] At room {} guardian {} is dead.", room.GetID(), arg1);
			}
		}
		else // IF (damage < guardian hp)
		{
			PrintLn("[RPC_DMG_GUARDIAN] At room {} - {} dmg to guardian {}.", room.GetID(), dmg, arg1);
		}

		// 브로드캐스트
		RpcEventDefault(room, user, RPC_DMG_GUARDIAN, arg0, arg1);
	}
	else // IF (guardian hp <= 0)
	{
		PrintLn("[RPC_DMG_GUARDIAN] At room {} - The guardian {} is already dead.", room.GetID(), arg1);
	}
}

void
ServerFramework::RpcEventOnGuardianPartDestructed(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	if (room.IsGameEnded())
	{
		return;
	}

	// arg1: index of the guardian
	if (arg1 < 0 or 3 <= arg1)
	{
		PrintLn("[RPC_DMG_GUARDIANS_PART] User {} tells wrong damaged guardian {}.", user.GetID(), arg1);

		return;
	}

	auto& guardian = room.sagaGuardians[arg1];

	auto& guardian_pt = guardian.myPartHealthPoints;
	auto rider_id = guardian.GetRiderId();

	// 곰 신체부위
	// 0: 파괴 X. 일반적인 경우 송수신 안함. 디버그 용으로 남김
	// 1(0): 오른쪽 팔
	// 2(1): 오른쪽 다리
	// 3(2): 왼쪽 팔
	// 4(3): 왼쪽 다리
	const std::int64_t part_id = arg0 - 1;

	guardian_pt[part_id] = 0;

	// 브로드캐스트
	RpcEventDefault(room, user, RPC_DMG_GUARDIANS_PART, part_id, arg1);

	if (guardian_pt[1] == 0 and guardian_pt[3] == 0)
	{
		constexpr float dmg_amount{ 9999.0f };
		std::int64_t dmg{};
		std::memcpy(&dmg, reinterpret_cast<const char*>(&dmg_amount), 4);

		RpcEventOnDamageToGuardian(room, user, RPC_DMG_GUARDIAN, dmg, arg1);

		PrintLn("[RPC_DMG_GUARDIANS_PART] Guardian {} at room {} is dead.", arg1, room.GetID());
	}
	else
	{
		//PrintLn("[RPC_DMG_GUARDIANS_PART] {}(th) part of {} at room {}.", arg0, arg1, room.GetID());
	}
}

void
ServerFramework::RpcEventOnRespawn(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	if (room.IsGameEnded())
	{
		return;
	}

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

			if (proc == RPC_RESPAWN_TIMER)
			{
				PrintLn("[RPC_RESPAWN] User {} is respawned.", user.GetID());
			}
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

			if (0 < cnt)
			{
				const auto ctx = AcquireSendContext();

				auto& mem_ctx = user.mainContext;

				user.SendGeneralData(*ctx, mem_ctx->GetRespawnTimePacketData(cnt));

				PrintLn("[RPC_RESPAWN_TIMER] User {}'s respawn time: {}.", user.GetID(), cnt);
			}
			else
			{
				RpcEventOnRespawn(room, user, RPC_RESPAWN_TIMER, arg0, arg1);
			}
		}
	);
}

void
ServerFramework::RpcEventOnPinataDestroyed(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	if (room.IsGameEnded())
	{
		return;
	}

	auto& winner = room.sagaWinner;

	const std::int32_t redscore = room.sagaTeamScores[0].load(std::memory_order_acquire);
	const std::int32_t bluscore = room.sagaTeamScores[1].load(std::memory_order_acquire);
	PrintLn("[RPC_DESTROY_CORE] Red team: {} | Blue team: {}.", redscore, bluscore);

	std::int64_t scores{};
	std::memcpy(reinterpret_cast<char*>(&scores), &redscore, 4);
	std::memcpy(reinterpret_cast<char*>(&scores) + 4, &bluscore, 4);

	// arg0: 점수
	// arg1: 박을 부순 팀의 식별자
	// 0 - nothing, 1 - red, 2 - blue
	if (0 < arg1)
	{
		std::int8_t prev_winner = 0;
		std::int8_t next_winner = static_cast<std::int8_t>(arg1);

		if (winner.compare_exchange_strong(prev_winner, next_winner))
		{
			RpcEventDefault(room, user, RPC_DESTROY_CORE, scores, next_winner);

			if (1 == arg1)
			{
				if (1 == winner.load(std::memory_order_relaxed))
				{
					PrintLn("[RPC_DESTROY_CORE] Red team wins at room {} - through user {}.", room.GetID(), user.GetID());
				}
			}
			else if (2 == arg1)
			{
				if (2 == winner.load(std::memory_order_relaxed))
				{
					PrintLn("[RPC_DESTROY_CORE] Blue team wins at room {} - through user {}.", room.GetID(), user.GetID());
				}
			}
			else
			{
				PrintLn("[RPC_DESTROY_CORE] User {} has received an invalid team id '{}' in room {}.", user.GetID(), arg1, room.GetID());
			}
		}
	}
	else
	{
		PrintLn("[RPC_DESTROY_CORE] User {} has received an invalid team id '{}' in room {}.", user.GetID(), arg1, room.GetID());
	}
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

	if (0 < cnt)
	{
		user.SendGeneralData(*AcquireSendContext(), room.MakeGameTimerPacket(cnt));
	}
	else
	{
		auto& winner = room.sagaWinner;

		const auto redscore = room.sagaTeamScores[0].load(std::memory_order_acquire);
		const auto bluscore = room.sagaTeamScores[1].load(std::memory_order_acquire);

		if (const auto curr_winner = winner.load(std::memory_order_acquire); 0 == curr_winner)
		{
			std::int8_t prev_winner{ 0 };
			std::int8_t next_winner{};

			PrintLn("[RPC_GAME_TIMER][RPC_GAME_END] Red team: {} | Blue team: {}.", redscore, bluscore);

			if (bluscore < redscore)
			{
				// The red team won
				next_winner = 1;

				PrintLn("[RPC_GAME_TIMER] Red team wins at room {} - through user {}.", room.GetID(), user.GetID());
			}
			else if (redscore < bluscore)
			{
				// The blue team won
				next_winner = 2;

				PrintLn("[RPC_GAME_TIMER] Blue team wins at room {} - through user {}.", room.GetID(), user.GetID());
			}
			else
			{
				// Both draws
				next_winner = 3;

				PrintLn("[RPC_GAME_TIMER] Two teams draws at room {} - through user {}.", room.GetID(), user.GetID());
			}

			if (winner.compare_exchange_strong(prev_winner, next_winner))
			{
				std::int64_t scores = 0;
				std::memcpy(reinterpret_cast<char*>(&scores), &redscore, 4);
				std::memcpy(reinterpret_cast<char*>(&scores) + 4, &bluscore, 4);

				RpcEventDefault(room, user, RPC_GAME_END, scores, next_winner);
			}
		}
		else
		{
			PrintLn("[RPC_GAME_TIMER] The game is done. The winner is {}.", curr_winner);
		}
	}
}

void
ServerFramework::RpcEventOnMiniBearActivated(iconer::app::Room& room
	, iconer::app::User& user
	, iconer::app::RpcProtocol proc
	, const std::int64_t& arg0, const std::int32_t& arg1)
{
	if (room.IsGameEnded())
	{
		return;
	}

	std::int8_t guardian_id{}; // 0 ~ 3
	std::int8_t guardian_part_id{}; // 0 ~ 3
	std::int16_t compressed_z{};
	std::int32_t casted_compressed_z{};
	float x{}, y{}, z{};

	std::memcpy(&x, reinterpret_cast<const char*>(&arg0), 4);
	std::memcpy(&y, reinterpret_cast<const char*>(&arg0) + 4, 4);

	std::memcpy(&guardian_id, reinterpret_cast<const char*>(&arg1), 1);
	std::memcpy(&guardian_part_id, reinterpret_cast<const char*>(&arg1) + 1, 1);
	std::memcpy(&compressed_z, reinterpret_cast<const char*>(&arg1) + 2, 2);
	casted_compressed_z = static_cast<std::int32_t>(compressed_z);
	std::memcpy(&z, reinterpret_cast<const char*>(&casted_compressed_z), 4);

	if (guardian_id < 0 or 4 <= guardian_id) UNLIKELY
	{
		PrintLn("[RPC_ACTIVE_GUARDIANS_PART] Invalid guardian '{}' has detected.", guardian_id);

		return;
	};

	if (guardian_part_id < 0 or 4 <= guardian_part_id) UNLIKELY
	{
		PrintLn("[RPC_ACTIVE_GUARDIANS_PART] Invalid part '{}' of guardian '{}' has detected.", guardian_part_id, guardian_id);

		return;
	};

	if (std::isnan(x) or std::isnan(y) or std::isnan(z)) UNLIKELY
	{
		PrintLn("[RPC_ACTIVE_GUARDIANS_PART] The part '{}' of guardian's position is invalid. ({}, {}, {})", guardian_part_id, guardian_id, x, y, z);

		return;
	};

	// arg0: x, y
	// arg1: guardian id (1 byte) | guardian part id (1 byte) | z (2 bytes) - compressed float
	//PrintLn("[RPC_DMG_PLYER] At room {} - {} dmg from {}.", room_id, dmg, arg1);

	auto& guardian = room.sagaGuardians[guardian_id];
	auto& part = guardian.myPartEntities[guardian_part_id];

	if (part.TryActivate())
	{
		part.x = x;
		part.y = y;
		part.z = z;

		/*
		auto [pk, size] = MakeRpc(RPC_ACTIVE_GUARDIANS_PART, user.GetID(), arg0, arg1);
		iconer::app::SendContext* const ctx = AcquireSendContext();
		ctx->myBuffer = std::move(pk);
		user.SendGeneralData(*ctx, pk.get(), size);
		*/

		PrintLn("[RPC_ACTIVE_GUARDIANS_PART] Guardian {}'s part {} is just activated on pos ({}, {}, {}).", guardian_id, guardian_part_id, x, y, z);
	}
	else
	{
		std::int64_t xy{};
		std::int32_t idz{};

		std::memcpy(&xy, &part.x, 4);
		std::memcpy(reinterpret_cast<char*>(&xy) + 4, &part.y, 4);
		std::memcpy(reinterpret_cast<char*>(&idz), &arg1, 2);

		std::memcpy(&casted_compressed_z, &part.z, 4);
		compressed_z = static_cast<std::int16_t>(casted_compressed_z);

		std::memcpy(reinterpret_cast<char*>(&idz) + 2, &compressed_z, 2);

		auto [pk, size] = MakeRpc(RPC_ACTIVE_GUARDIANS_PART, user.GetID(), xy, idz);

		iconer::app::SendContext* const ctx = AcquireSendContext();
		ctx->myBuffer = std::move(pk);

		user.SendGeneralData(*ctx, pk.get(), size);

		PrintLn("[RPC_ACTIVE_GUARDIANS_PART] Guardian {}'s part {} has already activated.", guardian_id, guardian_part_id);
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
