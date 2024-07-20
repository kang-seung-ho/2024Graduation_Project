module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]
#define ICONER_RPC_ENUM_ITEM(name) RPC_BEG_##name, RPC_END_##name
#include <chrono>
#include <atomic>

module Iconer.Framework;
import Iconer.App.Room;
import Iconer.App.SendContext;
import Iconer.App.PacketSerializer;
import <cstdint>;
import <print>;

enum class [[nodiscard]] RpcProtocol : std::uint8_t
{
	RPC_UNKNOWN = 0,
	ICONER_RPC_ENUM_ITEM(WALK),
	ICONER_RPC_ENUM_ITEM(RUN),
	ICONER_RPC_ENUM_ITEM(JUMP),
	ICONER_RPC_ENUM_ITEM(ATTACK_0), // Normal attack #1
	ICONER_RPC_ENUM_ITEM(ATTACK_1), // Normal attack #2
	ICONER_RPC_ENUM_ITEM(ATTACK_2), // Normal attack #3
	ICONER_RPC_ENUM_ITEM(ATTACK_3), // Normal attack #4
	ICONER_RPC_ENUM_ITEM(RIDE),
	RPC_POSITION,
	RPC_ROTATION,
	RPC_SKILL_0 = 100, // Ability #1
	RPC_SKILL_1, // Ability #2
	RPC_SKILL_2, // Ability #3
	RPC_SKILL_3, // Ability #4

	RPC_USE_ITEM_0 = 120,
	RPC_USE_ITEM_1,
	RPC_USE_ITEM_2,
	RPC_USE_ITEM_3,
	RPC_USE_ITEM_4,
	RPC_ASSIGN_ITEM_ID,
	RPC_DESTROY_ITEM_BOX,
	RPC_SPAWN_ITEM = 150,
	RPC_GRAB_ITEM,

	RPC_MAIN_WEAPON,
	RPC_CHANGE_HAND_ITEM,
	/// <summary>
	/// 클라에서 준비를 하면 서버에서 준비된 플레이어의 수를 보냄
	/// <para>-------------------------------------------------------------------------------</para>
	/// </summary>
	/// <param name="arg0">- 준비된 플레이어의 수</param>
	/// <param name="arg1">- 없음</param>
	RPC_NOTIFY_READY_COUNTER,
	RPC_DMG_PLYER,
	RPC_DMG_GUARDIAN,
	RPC_DMG_GUARDIANS_PART,
	RPC_DMG_BASE,
	RPC_DEAD,
	RPC_RESPAWN,
	RPC_RESPAWN_TIMER,

	RPC_CHECK_GAME_VICTORY,
	RPC_UPDATE_HEALTH,
	RPC_WEAPON_TIMER, // seconds
	RPC_GAME_TIMER, // seconds
	/// <summary>
	/// 클라에서 카운트 다운을 요청하면, 서버에서 남은 시간을 보냄
	/// <para>-------------------------------------------------------------------------------</para>
	/// </summary>
	/// <param name="arg0">- 준비된 플레이어의 수</param>
	/// <param name="arg1">- 없음</param>
	RPC_NOTIFY_GAME_COUNTDOWN,
};

namespace
{
	inline constexpr std::chrono::seconds gameAwaitPhasePeriod{ 31 };
	inline constexpr std::chrono::seconds respawnPeriod{ 5 };
}

#define MAKE_RPC_PACKET(_proc, _id, _arg0, _arg1) iconer::app::Serialize(iconer::app::PacketProtocol::SC_RPC, (std::int32_t)(_id), (_proc), (std::int64_t)(_arg0), (std::int32_t)(_arg1))

void
ServerFramework::EventOnRpc(iconer::app::User& user, std::byte* data)
{
	using enum RpcProtocol;

	iconer::app::Room* const room = user.GetRoom();

	if (nullptr != room) LIKELY
	{
		const RpcProtocol protocol = *reinterpret_cast<const RpcProtocol*>(data);
		data += sizeof(RpcProtocol);

		const std::int64_t arg0 = *reinterpret_cast<const std::int64_t*>(data);
		data += sizeof(std::int64_t);

		const std::int32_t arg1 = *reinterpret_cast<const std::int32_t*>(data);

		const auto user_id = user.GetID();
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

			auto was_cap = cap.load();
			auto new_cap = static_cast<size_t>(arg1);

			if (was_cap < new_cap)
			{
				if (cap.compare_exchange_strong(was_cap, new_cap))
				{
					items.reserve(new_cap);

					std::println("User {} claims {} item spawners.", user_id, new_cap);
				}
			}
			else
			{
				std::println("Room {}'s item list already has reserved {} items.", room_id, was_cap);
			}
		}
		break;

		// arg0 - nothing
		// arg1 - id
		case RPC_DESTROY_ITEM_BOX:
		{
			auto& items = room->sagaItemList;
			auto& ilock = room->sagaItemListLock;

			while (true)
			{
				bool flag = false;
				if (ilock.compare_exchange_strong(flag, true) or not room->IsTaken()) break;
			}

			if (not room->IsTaken() or 0 == room->GetMemberCount())
			{
				break;
			}

			const auto index = static_cast<size_t>(arg1);
			auto&& item = items[index];

			bool destroyed = false;
			if (item.isBoxDestroyed.compare_exchange_strong(destroyed, true))
			{
				room->Foreach
				(
					[&](iconer::app::User& member)
					{
						iconer::app::SendContext* const ctx = AcquireSendContext();
						auto [pk, size] = MAKE_RPC_PACKET(RPC_DESTROY_ITEM_BOX, user_id, arg0, arg1);

						ctx->myBuffer = std::move(pk);

						member.SendGeneralData(*ctx, size);
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
			auto& items = room->sagaItemList;
			auto& ilock = room->sagaItemListLock;

			while (true)
			{
				bool flag = false;
				if (ilock.compare_exchange_strong(flag, true) or not room->IsTaken()) break;
			}

			if (not room->IsTaken() or 0 == room->GetMemberCount())
			{
				break;
			}



			ilock = false;
		}
		break;

		// 
		// arg0 - 
		// arg1 - item type
		case RPC_USE_ITEM_0:
		{
			room->Foreach
			(
				[&](iconer::app::User& member)
				{
					iconer::app::SendContext* const ctx = AcquireSendContext();
					auto [pk, size] = MAKE_RPC_PACKET(RPC_USE_ITEM_0, user_id, arg0, arg1);

					ctx->myBuffer = std::move(pk);

					member.SendGeneralData(*ctx, size);
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
				room->Foreach
				(
					[&](iconer::app::User& member)
					{
						iconer::app::SendContext* const ctx = AcquireSendContext();
						auto [pk, size] = MAKE_RPC_PACKET(RPC_MAIN_WEAPON, user_id, arg0, arg1);

						// NOTICE: RPC_MAIN_WEAPON(arg0, 0)
						ctx->myBuffer = std::move(pk);

						member.SendGeneralData(*ctx, size);
					}
				);

				std::println("User {} changed weapon to {}.", user_id, arg0);
			}
			else
			{
				std::println("User {} could not change weapon to {}.", user_id, arg0);
			}
		}
		break;

		case RPC_BEG_RIDE:
		{
			if (not room->IsTaken() or 0 == room->GetMemberCount())
			{
				break;
			}

			std::println("[RPC_BEG_RIDE] at room {}.", room_id);

			// arg1: index of guardian
			if (arg1 < 0 or 3 <= arg1)
			{
				std::println("User {} tells wrong Guardian {}.", user_id, arg1);
				break;
			}

			auto& guardian = room->sagaGuardians[arg1];

			room->ProcessMember(&user, [&](iconer::app::Member& target)
				{
					const bool is_riding = target.isRidingGuardian.load(std::memory_order_acquire);

					if (0 < target.myHp and guardian.IsAlive() and not is_riding and guardian.TryRide(user_id))
					{
						std::println("User {} would ride the Guardian {}.", user_id, arg1);

						room->Foreach
						(
							[&](iconer::app::User& member)
							{
								iconer::app::SendContext* const ctx = AcquireSendContext();
								auto [pk, size] = MAKE_RPC_PACKET(RPC_BEG_RIDE, user_id, arg0, arg1);

								ctx->myBuffer = std::move(pk);

								member.SendGeneralData(*ctx, size);
							}
						);

						target.isRidingGuardian.store(true, std::memory_order_release);
					}
					else
					{
						std::println("User {} cannot ride the Guardian {}.", user_id, arg1);

						target.isRidingGuardian.store(false, std::memory_order_release);
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

			std::println("[RPC_END_RIDE] at room {}.", room_id);

			// arg1: index of guardian
			if (arg1 < 0 or 3 <= arg1)
			{
				std::println("User {} tells wrong guardian {}.", user_id, arg1);
				break;
			}

			auto& guardian = room->sagaGuardians[arg1];

			room->ProcessMember(&user, [&](iconer::app::Member& target)
				{
					const bool is_riding = target.isRidingGuardian.load(std::memory_order_acquire);

					if (is_riding and guardian.TryUnride(user_id))
					{
						std::println("User {} would unride from guardian {}.", user_id, arg1);

						room->Foreach
						(
							[&](iconer::app::User& member)
							{
								iconer::app::SendContext* const ctx = AcquireSendContext();
								auto [pk, size] = MAKE_RPC_PACKET(RPC_END_RIDE, user_id, arg0, arg1);

								ctx->myBuffer = std::move(pk);

								member.SendGeneralData(*ctx, size);
							}
						);

						target.isRidingGuardian.store(false, std::memory_order_release);
					}
					else
					{
						std::println("User {} was not riding guardian {}.", user_id, arg1);

						target.isRidingGuardian.store(is_riding, std::memory_order_release);
					}
				}
			);
		}
		break;

		case RPC_BEG_ATTACK_0:
		{
			std::println("[RPC_BEG_ATTACK_0] at room {}.", room_id);

			//if (0 < user.myHealth)
			{
				room->Foreach
				(
					[&](iconer::app::User& member)
					{
						iconer::app::SendContext* const ctx = AcquireSendContext();
						auto [pk, size] = MAKE_RPC_PACKET(RPC_BEG_ATTACK_0, user_id, arg0, arg1);

						ctx->myBuffer = std::move(pk);

						member.SendGeneralData(*ctx, size);
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
					auto& hp = target.myHp;

					float dmg{};
					std::memcpy(&dmg, reinterpret_cast<const char*>(&arg0), 4);
					std::println("[RPC_DMG_PLYER] At room {} - {} dmg to user {}.", room_id, dmg, arg1);

					if (0 < hp)
					{
						hp.fetch_sub(dmg);

						if (hp <= 0)
						{
							// TODO: 수호자 파괴

							// 하차 처리
							{
								//room->Foreach
								//(
								//	[&](iconer::app::User& member)
									//{
										//member.SendGeneralData(user_id, RPC_END_RIDE, arg0, arg1);
									//}
								//);
								//user.isRidingGuardian = false;
							}

							// 사망
							target.respawnTime = std::chrono::system_clock::now() + respawnPeriod;

							room->Foreach
							(
								[&](iconer::app::User& member)
								{
									iconer::app::SendContext* const ctx = AcquireSendContext();
									auto [pk, size] = MAKE_RPC_PACKET(RPC_DEAD, user_id, arg0, 0);

									ctx->myBuffer = std::move(pk);

									member.SendGeneralData(*ctx, size);
								}
							);
						}
						else
						{
							// 데미지 처리
							room->Foreach
							(
								[&](iconer::app::User& member)
								{
									iconer::app::SendContext* const ctx = AcquireSendContext();
									auto [pk, size] = MAKE_RPC_PACKET(RPC_DMG_PLYER, user_id, arg0, arg1);

									ctx->myBuffer = std::move(pk);

									member.SendGeneralData(*ctx, size);
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
			std::println("[RPC_DMG_GUARDIAN] at room {}.", room_id);

			float dmg{};
			std::memcpy(&dmg, reinterpret_cast<const char*>(&arg0), 4);
			std::println("[RPC_DMG_GUARDIAN] At room {} - {} dmg to guardian {}.", room_id, dmg, arg1);

			// 데미지 처리
			room->Foreach
			(
				[&](iconer::app::User& member)
				{
					iconer::app::SendContext* const ctx = AcquireSendContext();
					auto [pk, size] = MAKE_RPC_PACKET(RPC_DMG_GUARDIAN, user_id, arg0, arg1);

					ctx->myBuffer = std::move(pk);

					member.SendGeneralData(*ctx, size);
				}
			);
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
							auto [pk, size] = MAKE_RPC_PACKET(RPC_RESPAWN, user_id, arg0, arg1);

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

					if (now <= target.respawnTime)
					{
						// 
					}

					const auto gap = target.respawnTime - now;
					const size_t cnt = gap.count();
					std::println("User {}'s respawn time: {}.", user_id, cnt);

					if (0 < cnt)
					{
						iconer::app::SendContext* const ctx = AcquireSendContext();
						auto [pk, size] = MAKE_RPC_PACKET(RPC_RESPAWN_TIMER, user_id, cnt, arg1);

						ctx->myBuffer = std::move(pk);

						user.SendGeneralData(*ctx, size);
					}
					else
					{
						// RPC_RESPAWN랑 똑같은 처리
						auto& hp = target.myHp;
						hp = target.maxHp;

						room->Foreach
						(
							[&](iconer::app::User& member)
							{
								iconer::app::SendContext* const ctx = AcquireSendContext();
								auto [pk, size] = MAKE_RPC_PACKET(RPC_RESPAWN_TIMER, user_id, arg0, arg1);

								ctx->myBuffer = std::move(pk);

								user.SendGeneralData(*ctx, size);
							}
						);
					}
				}
			);
		}
		break;

		case RPC_DMG_GUARDIANS_PART:
		{
			std::println("[RPC_DMG_GUARDIANS_PART] at room {}.", room_id);

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
			const size_t cnt = gap.count();
			//std::println(L"\tRoom {}'s weapon phase: {}.", room_id, cnt);

			iconer::app::SendContext* const ctx = AcquireSendContext();
			auto [pk, size] = MAKE_RPC_PACKET(RPC_WEAPON_TIMER, user_id, cnt, 0);
			ctx->myBuffer = std::move(pk);

			user.SendGeneralData(*ctx, size);
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
			const size_t cnt = gap.count();

			iconer::app::SendContext* const ctx = AcquireSendContext();
			auto [pk, size] = MAKE_RPC_PACKET(RPC_GAME_TIMER, user_id, cnt, 0);
			ctx->myBuffer = std::move(pk);

			user.SendGeneralData(*ctx, size);
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
			const size_t cnt = gap.count();
			//std::println(L"\tRoom {}'s game time: {}.", room_id, cnt);

			iconer::app::SendContext* const ctx = AcquireSendContext();
			auto [pk, size] = MAKE_RPC_PACKET(RPC_NOTIFY_GAME_COUNTDOWN, user_id, cnt, 0);
			ctx->myBuffer = std::move(pk);

			user.SendGeneralData(*ctx, size);
		}
		break;

		default:
		{
			//std::println(L"\tRPC is proceed at room {}.", room_id);

			room->Foreach
			(
				[&](iconer::app::User& member)
				{
					iconer::app::SendContext* const ctx = AcquireSendContext();

					auto [pk, size] = MAKE_RPC_PACKET(protocol, user_id, arg0, arg1);
					ctx->myBuffer = std::move(pk);

					user.SendGeneralData(*ctx, size);
				}
			);
		}
		break;
		}
	};
}
