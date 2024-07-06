module;
#include <mutex>
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.App.Room;
import Iconer.App.User;

bool
iconer::app::Room::TryOccupy(iconer::app::User& user)
{
	auto& users_room = user.myRoom;
	auto& first = myMembers[0];

	std::int32_t zero{ 0 };
	std::int32_t one{ 1 };

	iconer::app::Room* self{ this };
	iconer::app::Room* empty_room{ nullptr };

	if (users_room.compare_exchange_strong(empty_room, this))
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
					}

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

	if (onFailedToOccupy.IsBound())
	{
		onFailedToOccupy.Broadcast(this, &user);
	}

	return false;
}

bool
iconer::app::Room::TryJoin(iconer::app::User& user)
{
	auto& users_room = user.myRoom;

	if (IsTaken()) LIKELY
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
							}

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
			} // IF (members < userLimits)
		} // FOREACH (myMembers)
	} // IF (IsTaken)

	if (onFailedToJoinUser.IsBound())
	{
		onFailedToJoinUser.Broadcast(this, &user);
	}

	return false;
}

bool
iconer::app::Room::Leave(iconer::app::User& user, bool notify)
noexcept
{
	auto& ctx = user.roomContext;

	auto is_taken = isTaken.load(std::memory_order_acquire);
	auto cnt = memberCount.load(std::memory_order_acquire);

	bool removed{ false };

	while (not ctx->TryChangeOperation(TaskCategory::None, TaskCategory::OpLeaveRoom));

	for (auto& member : myMembers)
	{
		iconer::app::Room* self{ this };

		const auto stored_user = member.GetStoredUser();

		if (nullptr == stored_user) continue;


		user.myRoom.compare_exchange_strong(self, nullptr);

		if (member.ChangedToEmpty(&user))
		{
			//std::scoped_lock lock{ memberRemoverLock };

			memberCount.compare_exchange_strong(cnt, cnt - 1, std::memory_order_release);

			if (notify and onUserLeft.IsBound())
			{
				onUserLeft.Broadcast(this, &user, cnt - 1);

				notify = false; // Notifty only once
			}

			removed = true;
		}
	}

	if (removed and 0 == memberCount)
	{
		// must be run
		if (onDestroyed.IsBound())
		{
			onDestroyed.Broadcast(this);
		}

		isTaken.store(false, std::memory_order_release);
	}

	ctx->TryChangeOperation(TaskCategory::OpLeaveRoom, TaskCategory::None);

	return removed;
}
