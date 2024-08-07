module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]
#include <chrono>
#include <atomic>

module Iconer.Framework;
import Iconer.App.Room;
import Iconer.App.SendContext;
import Iconer.App.PacketSerializer;
import Iconer.App.RpcProtocols;
import Iconer.App.GameSession;
import <cstdint>;
import <print>;
//import <coroutine>;

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

#define MAKE_RPC_PACKET(_proc, _id, _arg0, _arg1) \
iconer::app::Serialize(iconer::app::PacketProtocol::SC_RPC, (std::int32_t)(_id), (_proc), (std::int64_t)(_arg0), (std::int32_t)(_arg1))

void
ServerFramework::EventOnRpc(iconer::app::User& current_user, const std::byte* data)
{
	iconer::app::Room* const room = current_user.GetRoom();

	if (nullptr != room) LIKELY
	{
		if (not room->IsTaken() or 0 == room->GetMemberCount()) UNLIKELY
		{
			return;
		};

		const RpcProtocol protocol = *reinterpret_cast<const RpcProtocol*>(data);
		data += sizeof(RpcProtocol);

		std::int64_t arg0 = *reinterpret_cast<const std::int64_t*>(data);
		data += sizeof(std::int64_t);

		const std::int32_t arg1 = *reinterpret_cast<const std::int32_t*>(data);

		const auto user_id = static_cast<std::int32_t>(current_user.GetID());
		const auto room_id = room->GetID();

		const auto Broadcast = [this, room]
		(iconer::app::RpcProtocol proc, const std::int32_t& id, const std::int64_t& arg0, const std::int32_t& arg1)
			{
				auto [pk, size] = MakeSharedRpc(proc, id, arg0, arg1);

				room->Foreach
				(
					[&](iconer::app::User& member)
					{
						iconer::app::SendContext* const ctx = AcquireSendContext();

						ctx->mySharedBuffer = pk;

						member.SendGeneralData(*ctx, pk.get(), size);
					}
				);
			};

		switch (protocol)
		{
		case RPC_ASSIGN_ITEM_ID:
		{
			PrintLn("User {} claims {} item spawners.", user_id, arg1);
		}
		break;

		case RPC_BEG_RIDE:
		{
			PrintLn("[RPC_BEG_RIDE] At room {}.", room_id);

			// arg1: index of guardian
			if (arg1 < 0 or 3 <= arg1)
			{
				PrintLn("User {} tells wrong Guardian {}.", user_id, arg1);
				break;
			}

			auto& guardian = room->sagaGuardians[arg1];

			room->ProcessMember(&current_user, [&](iconer::app::SagaPlayer& target)
				{
					auto& rider = target.ridingGuardianId;

					const auto rider_id = rider.load(std::memory_order_acquire);
					const auto hp = target.myHp.load(std::memory_order_acquire);

					if (0 < hp and guardian.IsAlive() and guardian.TryRide(user_id))
					{
						std::int32_t pre_guardian_id = -1;

						if (rider.compare_exchange_strong(pre_guardian_id, arg1, std::memory_order_release))
						{
							Broadcast(RPC_BEG_RIDE, user_id, arg0, arg1);

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
						PrintLn("User {} cannot ride the Guardian {}.", user_id, arg1);

						std::int32_t pre_guardian_id = arg1;

						// rollback
						rider.compare_exchange_strong(pre_guardian_id, -1, std::memory_order_release);
					}
				}
			);
		}
		break;

		// arg0: nothing
		// arg1: index of guardian
		case RPC_END_RIDE:
		{
			PrintLn("[RPC_END_RIDE] At room {}.", room_id);

			if (arg1 < 0 or 3 <= arg1)
			{
				PrintLn("User {} tells wrong guardian {}.", user_id, arg1);
				break;
			}

			auto& guardian = room->sagaGuardians[arg1];

			room->ProcessMember(&current_user, [&](iconer::app::SagaPlayer& target)
				{
					auto& rider = target.ridingGuardianId;

					const auto rider_id = rider.load(std::memory_order_acquire);
					std::int32_t pre_guardian_id = arg1;

					if (guardian.TryUnride(user_id))
					{
						Broadcast(RPC_END_RIDE, user_id, arg0, arg1);

						rider.compare_exchange_strong(pre_guardian_id, -1, std::memory_order_release);

						PrintLn("User {} would unride from guardian {}.", user_id, arg1);
					}
					else
					{
						std::int32_t pre_guardian_id = arg1;
						rider.compare_exchange_strong(pre_guardian_id, -1, std::memory_order_release);

						PrintLn("User {} was not riding guardian {}.", user_id, arg1);
					}
				}
			);
		}
		break;

		// arg0: 플레이어가 받은 피해량(4바이트 부동소수점) | 플레이어 상태 (곰/인간) (4바이트 정수)
		// arg1: 플레이어에게 피해를 준 개체의 식별자 - 플레이어의 ID, 수호자의 순번
		case RPC_DMG_PLYER:
		{
			room->ProcessMember(&current_user, [&](iconer::app::SagaPlayer& target)
				{
					float dmg{};
					std::memcpy(&dmg, reinterpret_cast<const char*>(&arg0), 4);
					PrintLn("[RPC_DMG_PLYER] At room {} - {} dmg from {}.", room_id, dmg, arg1);

					auto& hp = target.myHp;
					auto& rider = target.ridingGuardianId;

					const auto rider_id = rider.load(std::memory_order_acquire);

					target.respawnTime = std::chrono::system_clock::now() + respawnPeriod;

					if (rider_id == -1)
					{
						// 최초로 체력이 0 이하일 때만 실행
						if (0 < hp.load(std::memory_order_acquire))
						{
							// 사망
							if (hp.fetch_sub(dmg, std::memory_order_acq_rel) - dmg <= 0)
							{
								Broadcast(RPC_DEAD, user_id, arg0, arg1);

								// 점수 증가
								if (arg1 != -2)
								{
									if (target.team_id == ESagaPlayerTeam::Red)
									{
										room->sagaTeamScores[1].fetch_add(1, std::memory_order_acq_rel);
									}
									else if (target.team_id == ESagaPlayerTeam::Blu)
									{
										room->sagaTeamScores[0].fetch_add(1, std::memory_order_acq_rel);
									}
								}

								rider.store(-1, std::memory_order_release);
							}
							else
							{
								// 데미지 처리
								Broadcast(RPC_DMG_PLYER, user_id, arg0, arg1);
							}
						} // IF (0 < hp)
					}
					else // IF NOT (rider_id is -1)
					{
						auto& guardian = room->sagaGuardians[rider_id];
						auto& guardian_hp = guardian.myHp;

						auto guardian_hp_value = guardian_hp.load(std::memory_order_acquire);

						if (0 < guardian_hp_value)
						{
							// 탑승했던 곰 사망
							if (guardian_hp.fetch_sub(dmg, std::memory_order_acq_rel) - dmg <= 0)
							{
								// 하차 처리
								guardian.TryUnride(user_id);
								guardian.myStatus = iconer::app::SagaGuardianState::Dead;
								target.ridingGuardianId.store(-1, std::memory_order_release);

								// 점수 증가
								if (target.team_id == ESagaPlayerTeam::Red)
								{
									room->sagaTeamScores[1].fetch_add(1, std::memory_order_acq_rel);
								}
								else if (target.team_id == ESagaPlayerTeam::Blu)
								{
									room->sagaTeamScores[0].fetch_add(1, std::memory_order_acq_rel);
								}

								// 하차 처리
								Broadcast(RPC_END_RIDE, user_id, arg0, rider_id);

								// 사망 피해량 브로드캐스트
								Broadcast(RPC_DMG_GUARDIAN, user_id, arg0, arg1);
							}
							else
							{
								Broadcast(RPC_DMG_GUARDIAN, user_id, arg0, arg1);
							}
						} // IF (0 < guardian hp)
					} // IF NOT (rider_id is -1)
				} // ProcessMember
			);
		}
		break;

		// arg0: 플레이어가 준 피해량 (4바이트 부동소수점)
		// arg1: 현재 수호자의 식별자
		case RPC_DMG_GUARDIAN:
		{
			// arg1: index of the guardian
			if (arg1 < 0 or 3 <= arg1)
			{
				PrintLn("User {} tells wrong damaged guardian {}.", user_id, arg1);
				break;
			}

			// arg0: damages to the guardian (4 bytes)
			float dmg{};
			std::memcpy(&dmg, reinterpret_cast<const char*>(&arg0), 4);
			// arg0: destructed body parts (4 bytes, 1 byte per part)
			std::int32_t parts{};
			std::memcpy(&parts, reinterpret_cast<const char*>(&arg0) + 4, 4);

			auto& guardian = room->sagaGuardians[arg1];
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

					// 하차 처리
					if (-1 != rider_id)
					{
						guardian.TryUnride(rider_id);

						auto [pk, size] = MakeSharedRpc(RPC_END_RIDE, rider_id, 0, arg1);

						constexpr std::int32_t killIncrement = 3;

						room->Foreach
						(
							[&](iconer::app::SagaPlayer& member)
							{
								const auto user_ptr = member.GetStoredUser();
								if (nullptr == user_ptr) return;

								iconer::app::SendContext* const ctx = AcquireSendContext();
								ctx->mySharedBuffer = pk;

								if (rider_id == user_ptr->GetID())
								{
									// NOTICE: 자기 팀의 점수 증가
									if (member.team_id == ESagaPlayerTeam::Red)
									{
										room->sagaTeamScores[0].fetch_add(killIncrement, std::memory_order_acq_rel);
									}
									else if (member.team_id == ESagaPlayerTeam::Blu)
									{
										room->sagaTeamScores[1].fetch_add(killIncrement, std::memory_order_acq_rel);
									}

									member.ridingGuardianId.compare_exchange_strong(rider_id, -1, std::memory_order_acq_rel);
								}

								user_ptr->SendGeneralData(*ctx, pk.get(), size);
							}
						);

						Broadcast(RPC_DMG_GUARDIAN, user_id, arg0, arg1);
					}
					else // IF (rider id is -1)
					{
						Broadcast(RPC_DMG_GUARDIAN, user_id, arg0, arg1);

						constexpr std::int32_t killIncrement = 1;

						room->Foreach
						(
							[&](iconer::app::SagaPlayer& member) noexcept
							{
								const auto user_ptr = member.GetStoredUser();
								if (nullptr == user_ptr) return;

								// 반드시 점수 증가
								// NOTICE: 자기 팀의 점수 증가
								//if (rider_id == user_ptr->GetID())
								{
									if (member.team_id == ESagaPlayerTeam::Red)
									{
										room->sagaTeamScores[0].fetch_add(killIncrement, std::memory_order_acq_rel);
									}
									else if (member.team_id == ESagaPlayerTeam::Blu)
									{
										room->sagaTeamScores[1].fetch_add(killIncrement, std::memory_order_acq_rel);
									}
								}
							}
						);
					}
				}
				else
				{
					Broadcast(RPC_DMG_GUARDIAN, user_id, arg0, arg1);
				}
			}
			else // IF (guardian hp <= 0)
			{
				PrintLn("[RPC_DMG_GUARDIAN] At room {} - The guardian {} is already dead.", room_id, arg1);
			}
		}
		break;

		// arg0: 파괴 부위
		// arg1: 현재 수호자의 식별자
		case RPC_DMG_GUARDIANS_PART:
		{
			// arg1: index of the guardian
			if (arg1 < 0 or 3 <= arg1)
			{
				PrintLn("[RPC_DMG_GUARDIANS_PART] User {} tells wrong damaged guardian {}.", user_id, arg1);
				break;
			}

			PrintLn("[RPC_DMG_GUARDIANS_PART] {}(th) part of {} at room {}.", arg0, arg1, room_id);

			auto& guardian = room->sagaGuardians[arg1];
			auto& guardian_hp = guardian.myHp;
			auto& guardian_pt = guardian.myParts;
			auto rider_id = guardian.GetRiderId();

			// 곰 신체부위
			// 0: 파괴 X. 일반적인 경우 송수신 안함. 디버그 용으로 남김
			// 1(0): 오른쪽 팔
			// 2(1): 오른쪽 다리
			// 3(2): 왼쪽 팔
			// 4(3): 왼쪽 다리
			guardian_pt[arg0 - 1] = 0;

			Broadcast(RPC_DMG_GUARDIANS_PART, user_id, arg0 - 1, arg1);

			if (guardian_pt[1] == 0 and guardian_pt[3] == 0)
			{
				//guardian.myStatus = iconer::app::SagaGuardianState::Dead;
				//guardian_hp.store(0, std::memory_order_release);

				//auto rider_id = guardian.GetRiderId();
				//guardian.TryUnride(rider_id);
				PrintLn("[RPC_DMG_GUARDIANS_PART] Guardian {} at room {} is dead.", arg1, room_id);

				if (-1 != rider_id)
				{
					const auto rider = userManager.FindUser(static_cast<size_t>(rider_id));

					if (nullptr != rider)
					{
						room->ProcessMember(rider, [&](iconer::app::SagaPlayer& target)
							{
								//target.ridingGuardianId.compare_exchange_strong(rider_id, -1);
							}
						);

						//guardian.TryUnride(rider_id);
					}
					else
					{
						guardian.TryUnride(rider_id);
					}
				}

				//Broadcast(RPC_DMG_GUARDIAN, user_id, 9999, arg1);
			}
		}
		break;

		// 일반적인 경우 실행안됨
		// 오직 ExecuteRespawnViaRpc 메서드로부터만 수신됨
		case RPC_RESPAWN:
		{
			room->ProcessMember(&current_user, [&](iconer::app::SagaPlayer& target)
				{
					auto& hp = target.myHp;
					hp = iconer::app::SagaPlayer::maxHp;

					Broadcast(RPC_RESPAWN, user_id, arg0, arg1);
				}
			);
		}
		break;

		case RPC_RESPAWN_TIMER:
		{
			room->ProcessMember(&current_user, [&](iconer::app::SagaPlayer& target)
				{
					const auto now = std::chrono::system_clock::now();

					const auto gap = target.respawnTime - now;
					const auto cnt = gap.count();
					PrintLn("User {}'s respawn time: {}.", user_id, cnt);

					if (0 < cnt)
					{
						iconer::app::SendContext* const ctx = AcquireSendContext();
						auto [pk, size] = MakeRpc(RPC_RESPAWN_TIMER, user_id, cnt, arg1);

						ctx->myBuffer = std::move(pk);

						current_user.SendGeneralData(*ctx, size);
					}
					else
					{
						// RPC_RESPAWN과 같은 처리
						auto& hp = target.myHp;
						hp.store(iconer::app::SagaPlayer::maxHp, std::memory_order_release);

						Broadcast(RPC_RESPAWN, user_id, arg0, arg1);
					}
				}
			);
		}
		break;

		case RPC_WEAPON_TIMER:
		{
			const auto now = std::chrono::system_clock::now();

			const auto gap = room->selectionPhaseTime - now;
			const size_t cnt = gap.count();
			//PrintLn(L"\tRoom {}'s weapon phase: {}.", room_id, cnt);

			iconer::app::SendContext* const ctx = AcquireSendContext();
			auto [pk, size] = MakeRpc(RPC_WEAPON_TIMER, user_id, cnt, 0);
			ctx->myBuffer = std::move(pk);

			current_user.SendGeneralData(*ctx, size);
		}
		break;

		// TODO: 박 체력 동기화
		// arg0: 박에 준 피해량
		// arg1: 박의 팀 식별자 (0: 팀 X, 1: 빨강, 2: 파랑)
		case RPC_DMG_BASE:
		{
			//PrintLn("[RPC_DESTROY_CORE] Red team: {} | Blue team: {}.", redscore, bluscore);

			room->ProcessMember(&current_user, [&](iconer::app::SagaPlayer& target)
				{
				}
			);
		}
		break;

		// arg0 : 점수
		// arg1 : 박을 부순 팀의 식별자
		case RPC_DESTROY_CORE:
		{
			auto& winner = room->sagaWinner;

			const std::int32_t redscore = room->sagaTeamScores[0].load(std::memory_order_acquire);
			const std::int32_t bluscore = room->sagaTeamScores[1].load(std::memory_order_acquire);
			PrintLn("[RPC_DESTROY_CORE] Red team: {} | Blue team: {}.", redscore, bluscore);

			std::memcpy(reinterpret_cast<char*>(&arg0), &redscore, 4);
			std::memcpy(reinterpret_cast<char*>(&arg0) + 4, &bluscore, 4);

			room->ProcessMember(&current_user, [&](iconer::app::SagaPlayer& target)
				{
					std::int8_t prev_winner = 0;

					if (target.team_id == ESagaPlayerTeam::Red)
					{
						if (winner.compare_exchange_strong(prev_winner, 1))
						{
							Broadcast(RPC_DESTROY_CORE, user_id, arg0, 1);

							PrintLn("[RPC_DESTROY_CORE] Red team wins at room {} - through user {}.", room_id, user_id);
						}
					}
					else if (target.team_id == ESagaPlayerTeam::Blu)
					{
						if (winner.compare_exchange_strong(prev_winner, 2))
						{
							Broadcast(RPC_DESTROY_CORE, user_id, arg0, 2);

							PrintLn("[RPC_DESTROY_CORE] Blue team wins at room {} - through user {}.", room_id, user_id);
						}
					}
					else
					{
						PrintLn("[RPC_DESTROY_CORE] User {} has an invalid team id in room {}.", room_id, user_id);
					}
				}
			);
		}
		break;

		default:
		{
			const size_t protocol_index = static_cast<size_t>(protocol);
			auto& processor = rpcProcessors[protocol_index];

			std::invoke(processor, this, *room, current_user, protocol, arg0, arg1);
		}
		break;
		}
	}
	else
	{
		PrintLn(L"\tUser {} has no room!", current_user.GetID());
	};
}
