module;
#include <cstddef>
#include <memory>
#include <utility>
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.App.Room;
import iconer.Utility.Serializer;
import Iconer.App.User;
import Iconer.App.PacketSerializer;

void
iconer::app::Room::Initialize()
{
	isDirty = true;

	(void)SerializeAt(precachedMemberListData, PacketProtocol::SC_RESPOND_USERS, 0ULL);
}

bool
iconer::app::Room::TryOccupy(iconer::app::Room::reference user)
{
	auto& users_room = user.myRoom;
	auto& first = myMembers[0];

	std::int32_t zero{ 0 };
	std::int32_t one{ 1 };

	iconer::app::Room* self{ this };
	iconer::app::Room* empty_room{ nullptr };

	if (users_room.compare_exchange_strong(empty_room, this)) LIKELY
	{
		if (memberCount.compare_exchange_strong(zero, 1, std::memory_order_acq_rel)) LIKELY
		{
			if (first.TryJoin(&user)) LIKELY
			{
				if (ChangedIsTaken(false, true)) LIKELY
				{
					if (onOccupied.IsBound())
					{
						onOccupied.Broadcast(this, &user);
					};

					/// NOTICE: set team here (occupy)
					first.team_id = user.GetID() % 2 == 0 ? ESagaPlayerTeam::Red : ESagaPlayerTeam::Blu;
					myState = RoomState::Idle;

					isDirty = true;
					return true;
				}
				else UNLIKELY
				{
					// rollback
					(void)first.ChangedToEmpty(&user);
					memberCount.compare_exchange_strong(one, 0);
					users_room.compare_exchange_strong(self, nullptr);
				};
			}
			else UNLIKELY
			{
				// rollback
				memberCount.compare_exchange_strong(one, 0);
				users_room.compare_exchange_strong(self, nullptr);
			};
		}
		else UNLIKELY
		{
			// rollback
			users_room.compare_exchange_strong(self, nullptr);
		};
	};

	return false;
}

bool
iconer::app::Room::TryJoin(iconer::app::Room::reference user)
{
	auto& users_room = user.myRoom;

	if (IsTaken() and IsIdle()) LIKELY
	{
		for (auto& member : myMembers)
		{
			auto cnt = memberCount.load(std::memory_order_acquire);
			auto ncnt = cnt + 1;

			if (cnt < userLimits) LIKELY
			{
				iconer::app::Room * self{ this };
				iconer::app::Room* empty_room{ nullptr };

				if (memberCount.compare_exchange_strong(cnt, ncnt, std::memory_order_release)) LIKELY
				{
					if (users_room.compare_exchange_strong(empty_room, this)) LIKELY
					{
						if (member.TryJoin(&user)) LIKELY
						{
							if (onUserJoined.IsBound())
							{
								onUserJoined.Broadcast(this, &user, ncnt);
							};

							/// NOTICE: set team here (join)
							member.team_id = user.GetID() % 2 == 0 ? ESagaPlayerTeam::Red : ESagaPlayerTeam::Blu;

							isDirty.store(true, std::memory_order_release);
							return true;
						}
						else UNLIKELY // IF (TryJoin)
						{
							// rollback and continue
							(void)member.ChangedToEmpty(&user);
							users_room.compare_exchange_strong(self, nullptr);
							memberCount.compare_exchange_strong(ncnt, cnt);
						};
					}
					else UNLIKELY // IF (CMPXHG(users_room))
					{
						// rollback and continue
						(void)member.ChangedToEmpty(&user);
						users_room.compare_exchange_strong(self, nullptr);
						memberCount.compare_exchange_strong(ncnt, cnt);
					};
				};
			}; // IF (members < userLimits)
		} // FOREACH (myMembers)
	}; // IF (IsTaken)

	if (onFailedToJoinUser.IsBound())
	{
		onFailedToJoinUser.Broadcast(this, &user);
	}

	return false;
}

bool
iconer::app::Room::Leave(iconer::app::Room::reference user, bool notify)
noexcept
{
	auto& ctx = user.roomContext;

	auto is_taken = isTaken.load(std::memory_order_acquire);

	bool removed{ false };

	//while (not ctx->TryChangeOperation(TaskCategory::None, TaskCategory::OpLeaveRoom));

	for (auto& member : myMembers)
	{
		auto cnt = memberCount.load(std::memory_order_acquire);

		iconer::app::Room* self{ this };

		const auto stored_user = member.GetStoredUser();

		if (nullptr == stored_user) continue;

		if (member.ChangedToEmpty(&user))
		{
			const auto user_id = user.GetID();
			auto& rider = member.ridingGuardianId;
			const auto rider_id = rider.load(std::memory_order_acquire);

			if (rider_id != -1)
			{
				auto& guardian = sagaGuardians[rider_id];

				guardian.TryUnride(user_id);
				rider.store(-1, std::memory_order_release);
			}

			//std::scoped_lock lock{ memberRemoverLock };

			if (notify and onUserLeft.IsBound())
			{
				onUserLeft.Broadcast(this, &user, cnt - 1);

				notify = false; // Notifty only once
			}

			memberCount.compare_exchange_strong(cnt, cnt - 1, std::memory_order_release);
			isDirty.store(true, std::memory_order_release);
			removed = true;
		}

		user.myRoom.compare_exchange_strong(self, nullptr);
	}

	if (0 == memberCount) LIKELY
	{
		if (onDestroyed.IsBound())
		{
			onDestroyed.Broadcast(this);
		}

		for (auto& item : sagaItemList)
		{
			item.isAvailable = true;
			item.isBoxDestroyed = false;
		}

		sagaItemListSize = 0;
		sagaItemListLock = false;

		for (auto& score : sagaTeamScores)
		{
			score = 0;
		}

		for (auto& guardian : sagaGuardians)
		{
			guardian.myHp = guardian.maxHp;
			guardian.myStatus = SagaGuardianState::Idle;
		}

		for (auto& member : myMembers)
		{
			member.team_id = ESagaPlayerTeam::Unknown;
			member.myWeapon = 0;
			member.isReady = 0;
			member.myHp = Member::maxHp;
			member.ridingGuardianId = -1;
		}

		sagaWinner = 0;

		myTitle.clear();
		myState.store(RoomState::Idle, std::memory_order_release);

		isTaken.store(false, std::memory_order_release);
	};

	ctx->TryChangeOperation(TaskCategory::OpLeaveRoom, TaskCategory::None);

	return removed;
}

void
iconer::app::Room::SetMemberTeam(iconer::app::Room::const_reference user, bool is_red_team)
{
	for (auto& member : myMembers)
	{
		if (const auto member_ptr = member.GetStoredUser(); member_ptr == &user)
		{
			if (is_red_team)
			{
				member.team_id = ESagaPlayerTeam::Red;
			}
			else
			{
				member.team_id = ESagaPlayerTeam::Blu;
			}

			isDirty.store(true, std::memory_order_release);
		}
	}
}

std::span<const std::byte>
iconer::app::Room::MakeMemberListPacket()
{
	if (auto is_dirty = isDirty.load(std::memory_order_acquire); is_dirty)
	{
		auto seek = precachedMemberListData.data() + 3 + sizeof(size_t);

		// Serialize rooms
		size_t count{};

		for (auto& member : myMembers)
		{
			if (not member.IsAvailable()) continue;

			const auto user = member.GetStoredUser();

			constexpr auto len = iconer::app::Settings::nickNameLength;

			wchar_t serialized_name[len]{};
			const auto nickname = user->GetName().substr(0, len);
			std::copy(nickname.cbegin(), nickname.cend(), serialized_name);

			seek = iconer::util::Serialize(seek, static_cast<std::int32_t>(user->GetID()));
			seek = iconer::util::Serialize(seek, member.team_id);
			seek = iconer::util::Serialize(seek, serialized_name);

			++count;
		}

		precachedMemberListDataSize = 3 + sizeof(size_t) + SerializedMember::GetSerializedSize() * count;

		auto post = iconer::util::Serialize(precachedMemberListData.data() + 1, static_cast<std::int16_t>(precachedMemberListDataSize));
		iconer::util::Serialize(post, count);

		isDirty.compare_exchange_strong(is_dirty, false, std::memory_order_acq_rel);
	}

	return std::span<const std::byte>{ precachedMemberListData.data(), precachedMemberListDataSize };
}

std::pair<std::unique_ptr<std::byte[]>, std::int16_t>
iconer::app::Room::MakeMemberJoinedPacket(iconer::app::Room::const_reference user)
const
{
	for (auto& member : myMembers)
	{
		const auto member_ptr = member.GetStoredUser();

		if (member_ptr == &user)
		{
			constexpr auto len = iconer::app::Settings::nickNameLength;

			wchar_t serialized_name[len]{};
			const auto nickname = user.GetName().substr(0, len);
			std::copy(nickname.cbegin(), nickname.cend(), serialized_name);

			return Serialize(PacketProtocol::SC_ROOM_JOINED, static_cast<std::int32_t>(user.GetID()), member.team_id, serialized_name);
		}
	}

	return std::make_pair(std::unique_ptr<std::byte[]>(), 0);
}
