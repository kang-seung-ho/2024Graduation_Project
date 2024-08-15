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
									auto& team_id = target.myTeamId;
									const auto team = team_id.load(std::memory_order_acquire);

									if (team == ESagaPlayerTeam::Red)
									{
										room->sagaTeamScores[1].fetch_add(1, std::memory_order_acq_rel);
									}
									else if (team == ESagaPlayerTeam::Blu)
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
								auto& team_id = target.myTeamId;
								const auto team = team_id.load(std::memory_order_acquire);

								// 하차 처리
								guardian.TryUnride(user_id);
								guardian.myStatus = iconer::app::SagaGuardianState::Dead;
								target.ridingGuardianId.store(-1, std::memory_order_release);

								// 점수 증가
								if (team == ESagaPlayerTeam::Red)
								{
									room->sagaTeamScores[1].fetch_add(1, std::memory_order_acq_rel);
								}
								else if (team == ESagaPlayerTeam::Blu)
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

		// 테스트 용
		// arg0: 승리한 팀의 ID (4바이트) | 승리한 팀의 점수(4바이트)
		// arg1: 승패 결정 수단
		//		수단:	0 - 피냐타 파괴
		//					1 - 게임 시간 종료
		case RPC_GAME_END:
		{


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
					auto& team_id = target.myTeamId;
					const auto team = team_id.load(std::memory_order_acquire);

					std::int8_t prev_winner = 0;

					if (team == ESagaPlayerTeam::Red)
					{
						if (winner.compare_exchange_strong(prev_winner, 1))
						{
							Broadcast(RPC_DESTROY_CORE, user_id, arg0, 1);

							PrintLn("[RPC_DESTROY_CORE] Red team wins at room {} - through user {}.", room_id, user_id);
						}
					}
					else if (team == ESagaPlayerTeam::Blu)
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
