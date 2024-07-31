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
import <cstdint>;
import <print>;

using iconer::app::RpcProtocol;
using enum iconer::app::RpcProtocol;

namespace
{
	inline constexpr std::chrono::seconds gameAwaitPhasePeriod{ 31 };
	inline constexpr std::chrono::seconds respawnPeriod{ 5 };

	template<typename Protocol>
	[[nodiscard]]
	constexpr std::pair<std::unique_ptr<std::byte[]>, std::int16_t>
		MakeRpc(Protocol&& proc, const std::int32_t& id, const std::int64_t& arg0, const std::int32_t& arg1)
	{
		return iconer::app::Serialize(iconer::app::PacketProtocol::SC_RPC, id, proc, arg0, arg1);
	}

	template<typename Protocol>
	[[nodiscard]]
	std::pair<std::shared_ptr<std::byte[]>, std::int16_t>
		MakeSharedRpc(Protocol&& proc, const std::int32_t& id, const std::int64_t& arg0, const std::int32_t& arg1)
	{
		auto [ptr, size] = iconer::app::Serialize(iconer::app::PacketProtocol::SC_RPC, id, proc, arg0, arg1);

		return std::make_pair(std::shared_ptr<std::byte[]>(ptr.release()), size);
	}
}

#define MAKE_RPC_PACKET(_proc, _id, _arg0, _arg1) \
iconer::app::Serialize(iconer::app::PacketProtocol::SC_RPC, (std::int32_t)(_id), (_proc), (std::int64_t)(_arg0), (std::int32_t)(_arg1))

void
ServerFramework::EventOnRpc(iconer::app::User& user, const std::byte* data)
{
	iconer::app::Room* const room = user.GetRoom();

	if (nullptr != room) LIKELY
	{
		const RpcProtocol protocol = *reinterpret_cast<const RpcProtocol*>(data);
		data += sizeof(RpcProtocol);

		const std::int64_t arg0 = *reinterpret_cast<const std::int64_t*>(data);
		data += sizeof(std::int64_t);

		const std::int32_t arg1 = *reinterpret_cast<const std::int32_t*>(data);

		const auto user_id = static_cast<std::int32_t>(user.GetID());
		const auto room_id = room->GetID();

		switch (protocol)
		{
		case RPC_ASSIGN_ITEM_ID:
		{
			if (not room->IsTaken() or 0 == room->GetMemberCount())
			{
				break;
			}

			auto& items = room->sagaItemList;
			auto& cap = room->sagaItemListSize;

			auto new_cap = static_cast<size_t>(arg1);

			PrintLn("User {} claims {} item spawners.", user_id, new_cap);
		}
		break;

		// arg0 - nothing
		// arg1 - id
		case RPC_DESTROY_ITEM_BOX:
		{
			auto& items = room->sagaItemList;

			if (not room->IsTaken() or 0 == room->GetMemberCount())
			{
				break;
			}

			const auto index = static_cast<size_t>(arg1);
			auto& item = items[index];

			bool destroyed = false;
			if (item.isBoxDestroyed.compare_exchange_strong(destroyed, true))
			{
				room->Foreach
				(
					[&](iconer::app::User& member)
					{
						iconer::app::SendContext* const ctx = AcquireSendContext();

						auto [pk, size] = MakeRpc(RPC_DESTROY_ITEM_BOX, user_id, arg0, arg1);

						ctx->myBuffer = std::move(pk);

						member.SendGeneralData(*ctx, size);
					}
				);
			}
		}
		break;

		// 
		// arg0 - nothing
		// arg1 - id
		case RPC_GRAB_ITEM:
		{
			auto& items = room->sagaItemList;

			if (not room->IsTaken() or 0 == room->GetMemberCount())
			{
				break;
			}

			if (arg1 < 0 or 200 <= arg1) break;

			const auto index = static_cast<size_t>(arg1);
			auto&& item = items[index];

			if (item.isBoxDestroyed)
			{
				bool grabbed = true;
				if (item.isAvailable.compare_exchange_strong(grabbed, false))
				{
					auto [pk, size] = MakeSharedRpc(RPC_GRAB_ITEM, user_id, arg0, arg1);

					room->Foreach
					(
						[&](iconer::app::User& member)
						{
							iconer::app::SendContext* const ctx = AcquireSendContext();

							ctx->mySharedBuffer = pk;

							member.SendGeneralData(*ctx, pk.get(), size);
						}
					);
				}
			}
		}
		break;

		// 
		// arg0 - 
		// arg1 - item type
		case RPC_USE_ITEM_0:
		{
			auto [pk, size] = MakeSharedRpc(RPC_USE_ITEM_0, user_id, arg0, arg1);

			room->Foreach
			(
				[&](iconer::app::User& member)
				{
					iconer::app::SendContext* const ctx = AcquireSendContext();

					ctx->mySharedBuffer = pk;

					member.SendGeneralData(*ctx, pk.get(), size);
				}
			);
		}
		break;

		case RPC_MAIN_WEAPON:
		{
			if (not room->IsTaken() or 0 == room->GetMemberCount())
			{
				break;
			}

			// Change the weapon first
			// arg0: kind of weapon
			// 0: lightsaber
			// 1: watergun
			// 2: hammer
			if (room->ProcessMember(&user, [&](iconer::app::Member& member)
				{
					member.myWeapon = static_cast<std::uint8_t>(arg0);
				}
			))
			{
				// Broadcast his weapon
				auto [pk, size] = MakeSharedRpc(RPC_MAIN_WEAPON, user_id, arg0, arg1);

				room->Foreach
				(
					[&](iconer::app::User& member)
					{
						iconer::app::SendContext* const ctx = AcquireSendContext();

						ctx->mySharedBuffer = pk;

						member.SendGeneralData(*ctx, pk.get(), size);
					}
				);

				PrintLn("User {} changed weapon to {}.", user_id, arg0);
			}
			else
			{
				PrintLn("User {} could not change weapon to {}.", user_id, arg0);
			}
		}
		break;

		case RPC_BEG_RIDE:
		{
			if (not room->IsTaken() or 0 == room->GetMemberCount())
			{
				break;
			}

			PrintLn("[RPC_BEG_RIDE] at room {}.", room_id);

			// arg1: index of guardian
			if (arg1 < 0 or 3 <= arg1)
			{
				PrintLn("User {} tells wrong Guardian {}.", user_id, arg1);
				break;
			}

			auto& guardian = room->sagaGuardians[arg1];

			room->ProcessMember(&user, [&](iconer::app::Member& target)
				{
					auto& rider = target.ridingGuardianId;

					const auto rider_id = rider.load(std::memory_order_acquire);
					const auto hp = target.myHp.load(std::memory_order_acquire);

					if (0 < hp and guardian.IsAlive() and guardian.TryRide(user_id))
					{
						std::int32_t pre_guardian_id = -1;

						if (rider.compare_exchange_strong(pre_guardian_id, arg1, std::memory_order_release))
						{
							PrintLn("User {} would ride the Guardian {}.", user_id, arg1);

							auto [pk, size] = MakeSharedRpc(RPC_BEG_RIDE, user_id, arg0, arg1);

							room->Foreach
							(
								[&](iconer::app::User& member)
								{
									iconer::app::SendContext* const ctx = AcquireSendContext();

									ctx->mySharedBuffer = pk;

									member.SendGeneralData(*ctx, pk.get(), size);
								}
							);
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

		case RPC_END_RIDE:
		{
			if (not room->IsTaken() or 0 == room->GetMemberCount())
			{
				break;
			}

			PrintLn("[RPC_END_RIDE] at room {}.", room_id);

			// arg1: index of guardian
			if (arg1 < 0 or 3 <= arg1)
			{
				PrintLn("User {} tells wrong guardian {}.", user_id, arg1);
				break;
			}

			auto& guardian = room->sagaGuardians[arg1];

			room->ProcessMember(&user, [&](iconer::app::Member& target)
				{
					auto& rider = target.ridingGuardianId;

					const auto rider_id = rider.load(std::memory_order_acquire);
					std::int32_t pre_guardian_id = arg1;

					if (guardian.TryUnride(user_id))
					{
						PrintLn("User {} would unride from guardian {}.", user_id, arg1);

						room->Foreach
						(
							[&](iconer::app::User& member)
							{
								iconer::app::SendContext* const ctx = AcquireSendContext();
								auto [pk, size] = MakeRpc(RPC_END_RIDE, user_id, arg0, arg1);

								ctx->myBuffer = std::move(pk);

								member.SendGeneralData(*ctx, size);
							}
						);

						rider.compare_exchange_strong(pre_guardian_id, -1, std::memory_order_release);
					}
					else
					{
						PrintLn("User {} was not riding guardian {}.", user_id, arg1);

						std::int32_t pre_guardian_id = arg1;
						rider.compare_exchange_strong(pre_guardian_id, -1, std::memory_order_release);
					}
				}
			);
		}
		break;

		case RPC_BEG_ATTACK_0:
		{
			PrintLn("[RPC_BEG_ATTACK_0] at room {}.", room_id);

			//if (0 < user.myHealth)
			{
				auto [pk, size] = MakeSharedRpc(RPC_BEG_ATTACK_0, user_id, arg0, arg1);

				room->Foreach
				(
					[&](iconer::app::User& member)
					{
						iconer::app::SendContext* const ctx = AcquireSendContext();

						ctx->mySharedBuffer = pk;

						member.SendGeneralData(*ctx, pk.get(), size);
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

			room->ProcessMember(&user, [&](iconer::app::Member& target)
				{
					float dmg{};
					std::memcpy(&dmg, reinterpret_cast<const char*>(&arg0), 4);
					PrintLn("[RPC_DMG_PLYER] At room {} - {} dmg to user {}.", room_id, dmg, arg1);

					auto& hp = target.myHp;
					auto& rider = target.ridingGuardianId;

					const auto rider_id = rider.load(std::memory_order_acquire);

					if (rider_id == -1)
					{
						if (0 < hp)
						{
							// 사망
							if (hp.fetch_sub(dmg, std::memory_order_acq_rel) - dmg <= 0)
							{
								// 하차 처리
								target.respawnTime = std::chrono::system_clock::now() + respawnPeriod;

								auto [pk, size] = MakeSharedRpc(RPC_END_RIDE, user_id, arg0, rider_id);

								room->Foreach
								(
									[&](iconer::app::User& member)
									{
										iconer::app::SendContext* const ctx = AcquireSendContext();

										ctx->mySharedBuffer = pk;

										member.SendGeneralData(*ctx, pk.get(), size);
									}
								);

								auto [pk2, size2] = MakeSharedRpc(RPC_DEAD, user_id, arg0, 0);

								room->Foreach
								(
									[&](iconer::app::User& member)
									{
										iconer::app::SendContext* const ctx = AcquireSendContext();

										ctx->mySharedBuffer = pk2;

										member.SendGeneralData(*ctx, pk2.get(), size2);
									}
								);

								// 점수 증가
								if (target.team_id == 0)
								{
									room->sagaTeamScores[1].fetch_add(1, std::memory_order_acq_rel);
								}
								else
								{
									room->sagaTeamScores[0].fetch_add(1, std::memory_order_acq_rel);
								}

								target.ridingGuardianId = -1;
							}
							else
							{
								auto [pk, size] = MakeSharedRpc(RPC_DMG_PLYER, user_id, arg0, arg1);

								// 데미지 처리
								room->Foreach
								(
									[&](iconer::app::User& member)
									{
										iconer::app::SendContext* const ctx = AcquireSendContext();

										ctx->mySharedBuffer = pk;

										member.SendGeneralData(*ctx, pk.get(), size);
									}
								);
							}
						} // IF (0 < hp)
					}
					else // IF NOT (rider_id is -1)
					{
						auto& guardian = room->sagaGuardians[rider_id];
						auto& guardian_hp = guardian.myHp;

						if (0 < guardian_hp)
						{
							// 탑승했던 곰 사망
							if (guardian_hp.fetch_sub(dmg) - dmg <= 0)
							{
								// 하차 처리
								guardian.TryUnride(user_id);
								guardian.myStatus = iconer::app::SagaGuardianState::Dead;
								target.ridingGuardianId = -1;

								// 점수 증가
								if (target.team_id == 0)
								{
									room->sagaTeamScores[1].fetch_add(1, std::memory_order_acq_rel);
								}
								else
								{
									room->sagaTeamScores[0].fetch_add(1, std::memory_order_acq_rel);
								}

								auto [pk, size] = MakeSharedRpc(RPC_END_RIDE, user_id, arg0, rider_id);

								room->Foreach
								(
								[&](iconer::app::User& member)
									{
										iconer::app::SendContext* const ctx = AcquireSendContext();

										ctx->mySharedBuffer = pk;

										member.SendGeneralData(*ctx, pk.get(), size);
									}
								);
							}
						}
						else
						{
							auto [pk, size] = MakeSharedRpc(RPC_DMG_GUARDIAN, user_id, arg0, arg1);

							room->Foreach
							(
								[&](iconer::app::User& member)
								{
									iconer::app::SendContext* const ctx = AcquireSendContext();

									ctx->mySharedBuffer = pk;

									member.SendGeneralData(*ctx, pk.get(), size);
								}
							);
						}
					}
				}
			);
		}
		break;

		case RPC_DMG_GUARDIAN:
		{

		}
		break;

		// 일반적인 경우 실행안됨
		// 오직 ExecuteRespawnViaRpc 메서드로부터만 수신됨
		case RPC_RESPAWN:
		{
			room->ProcessMember(&user, [&](iconer::app::Member& target)
				{
					auto& hp = target.myHp;
					hp = iconer::app::Member::maxHp;

					room->Foreach
					(
						[&](iconer::app::User& member)
						{
							iconer::app::SendContext* const ctx = AcquireSendContext();
							auto [pk, size] = MakeRpc(RPC_RESPAWN, user_id, arg0, arg1);

							ctx->myBuffer = std::move(pk);

							member.SendGeneralData(*ctx, size);
						}
					);
				}
			);
		}
		break;

		case RPC_RESPAWN_TIMER:
		{
			room->ProcessMember(&user, [&](iconer::app::Member& target)
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

						user.SendGeneralData(*ctx, size);
					}
					else
					{
						// RPC_RESPAWN과 같은 처리
						auto& hp = target.myHp;
						hp.store(target.maxHp, std::memory_order_release);

						auto [pk, size] = MakeSharedRpc(RPC_RESPAWN, user_id, arg0, arg1);

						room->Foreach
						(
							[&](iconer::app::User& member)
							{
								iconer::app::SendContext* const ctx = AcquireSendContext();

								ctx->mySharedBuffer = pk;

								user.SendGeneralData(*ctx, pk.get(), size);
							}
						);
					}
				}
			);
		}
		break;

		case RPC_DMG_GUARDIANS_PART:
		{
			PrintLn("[RPC_DMG_GUARDIANS_PART] at room {}.", room_id);

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

			user.SendGeneralData(*ctx, size);
		}
		break;

		case RPC_DESTROY_CORE:
		{

		}
		break;

		case RPC_GET_SCORE:
		{
			const auto redscore = room->sagaTeamScores[0].load(std::memory_order_relaxed);
			const auto bluscore = room->sagaTeamScores[1].load(std::memory_order_relaxed);

			auto [pk, size] = MakeSharedRpc(RPC_GET_SCORE, 0, redscore, bluscore);

			room->Foreach
			(
				[&](iconer::app::User& member)
				{
					iconer::app::SendContext* const ctx = AcquireSendContext();

					ctx->mySharedBuffer = pk;

					user.SendGeneralData(*ctx, pk.get(), size);
				}
			);
		}
		break;

		case RPC_GAME_TIMER:
		{
			const auto now = std::chrono::system_clock::now();

			const auto gap = room->gamePhaseTime - now;
			const auto cnt = gap.count();

			if (0 < cnt)
			{
			}
			else
			{
			}

			iconer::app::SendContext* const ctx = AcquireSendContext();
			auto [pk, size] = MakeRpc(RPC_GAME_TIMER, user_id, cnt, 0);
			ctx->myBuffer = std::move(pk);

			user.SendGeneralData(*ctx, size);
		}
		break;

		case RPC_NOTIFY_GAME_COUNTDOWN:
		{
			const auto now = std::chrono::system_clock::now();

			const auto gap = room->gamePhaseTime - now;
			const size_t cnt = gap.count();
			//PrintLn(L"\tRoom {}'s game time: {}.", room_id, cnt);

			iconer::app::SendContext* const ctx = AcquireSendContext();
			auto [pk, size] = MakeRpc(RPC_NOTIFY_GAME_COUNTDOWN, user_id, cnt, 0);
			ctx->myBuffer = std::move(pk);

			user.SendGeneralData(*ctx, size);
		}
		break;

		default:
		{
			//PrintLn(L"\tRPC is proceed at room {}.", room_id);

			room->Foreach
			(
				[&](iconer::app::User& member)
				{
					iconer::app::SendContext* const ctx = AcquireSendContext();

					auto [pk, size] = MakeRpc(protocol, user_id, arg0, arg1);
					ctx->myBuffer = std::move(pk);

					user.SendGeneralData(*ctx, size);
				}
			);
		}
		break;
		}
	};
}
