module;
#include <string_view>
#include <vector>
#include <atomic>

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

bool
demo::Framework::OnCreateGame(iconer::app::User& user)
{
	using enum iconer::app::AsyncOperations;

	const auto room_id = user.myRoomId.Load();
	if (-1 == room_id)
	{
		// rollback
		user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::Idle);

		// cannot prepare the game: The client is not in a room
		SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::NotInRoom);
	}
	else if (auto room = FindRoom(room_id); nullptr != room)
	{
		// make clients getting ready for game
		// for room master first
		if (not room->CanCreateGame())
		{
			// cannot prepare the game: The room is lack of member
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::LackOfMember);
		}
		else if (not user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::MakingGame))
		{
			// cannot prepare the game: The client is busy
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::ClientIsBusy);

		}
		else if (not room->TryGettingReady())
		{
			// rollback
			user.TryChangeState(iconer::app::UserStates::MakingGame, iconer::app::UserStates::InRoom);

			// cannot prepare the game: Not proper operation
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::InvalidOperation);
		}
		else if (not room->HasMember(user.GetID()))
		{
			// rollback
			user.myRoomId.CompareAndSet(room_id, -1);
			user.TryChangeState(iconer::app::UserStates::MakingGame, iconer::app::UserStates::Idle);
			room->TryCancelReady();

			// cannot prepare the game: The client has no room
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::NotInRoom);
		}
		else if (room->SetOperation(OpSpreadGameTicket);
			not Schedule(room, 0))
		{
			// rollback
			user.TryChangeState(iconer::app::UserStates::MakingGame, iconer::app::UserStates::InRoom);
			room->TryCancelReady();

			// cannot prepare the game: Server error occured
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::ServerError);
		}
		else
		{
			return user.TryChangeState(iconer::app::UserStates::MakingGame, iconer::app::UserStates::InRoom);
		}
	}
	else
	{
		// rollback
		if (user.myRoomId.CompareAndSet(room_id, -1))
		{
			user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::Idle);
		}

		// cannot start a game: The client has a invalid room
		SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::InvalidRoom);
	}

	return false;
}

void
demo::Framework::OnFailedToCreateGame(iconer::app::User& user)
noexcept
{
	if (const auto room_id = user.myRoomId.Load(); -1 != room_id)
	{
		if (auto room = FindRoom(room_id); nullptr != room)
		{
			room->TryCancelReady();
		}
	}
}

bool
demo::Framework::OnBroadcastGameTickets(iconer::app::Room& room)
{
	if (room.CanPrepareGame())
	{
		std::vector<iconer::app::User*> members{};
		members.reserve(iconer::app::Room::maxUsersNumberInRoom);

		room.ForEach
		(
			[&members](iconer::app::User& member)
			{
				members.push_back(std::addressof(member));
			}
		);

#define MUST_START true

#if MUST_START
		if (0 < members.size())
#else
		if (iconer::app::Room::minUsersNumberForGame <= members.size())
#endif
		{
			for (auto& member : members)
			{
				SEND(*member, SendMakeGameReadyPacket);
			}

			return true;
		}
		else
		{
			// rollback
			room.TryCancelReady();

			// cannot prepare the game: The room is lack of member
			for (auto& member : members)
			{
				SEND(*member, SendCannotStartGamePacket, iconer::app::GameContract::LackOfMember);
			}

			return false;
		}
	}
	else
	{
		// rollback
		room.TryCancelReady();

		return false;
	}
}

void
demo::Framework::OnFailedToBroadcastGameTickets(iconer::app::Room& room)
noexcept
{
	room.TryCancelReady();
}

bool
demo::Framework::OnSentGameTicket(iconer::app::User& user)
{
	using enum iconer::app::AsyncOperations;

	if (const auto room_id = user.myRoomId.Load(); -1 != room_id)
	{
		if (auto room = FindRoom(room_id); nullptr != room)
		{
			auto& cnt_ref = room->proceedMemberCount;
			auto& failed_ref = room->isGameReadyFailed;

			auto cnt = cnt_ref.Load(std::memory_order_acquire);
			auto failed = failed_ref.Load(std::memory_order_acquire);

			if (failed)
			{
				// cannot start a game: Other clients has been failed
				SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::OtherClientFailed);
			}

			if (not room->HasMember(user.GetID()) or not room->CanPrepareGame())
			{
				// mark failed at anytime
				failed_ref.Store(true, std::memory_order_relaxed);
				failed = true;

				// cannot start a game: The room is unstable
				SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::UnstableRoom);
			}

			// check condition using plain fields
			if (room->GetMembersCount() <= cnt + 1)
			{
				if (failed)
				{
					if (room->TryCancelReady(iconer::app::RoomStates::Closing))
					{
						room->SetOperation(OpCloseGame);
						(void)Schedule(room, 0);
					}
				}

				// rollback atomics
				cnt_ref.Store(0, std::memory_order_release);
				failed_ref.Store(false, std::memory_order_release);
			}
			else
			{
				// increase and release
				cnt_ref.CompareAndSet(cnt, cnt, std::memory_order_release);
			}

			return not failed;
		}
		else
		{
			// rollback
			if (user.myRoomId.CompareAndSet(room_id, -1))
			{
				user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::Idle);
			}

			// cannot start a game: The client has a invalid room
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::InvalidRoom);
		}
	}
	else
	{
		// cannot start a game: The client is not in a room
		SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::NotInRoom);
	}

	return false;
}

void
demo::Framework::OnFailedToSendGameTicket(iconer::app::User& user)
noexcept
{
	using enum iconer::app::AsyncOperations;

	if (const auto room_id = user.myRoomId.Load(); -1 != room_id)
	{
		if (auto room = FindRoom(room_id); nullptr != room)
		{
			auto& cnt_ref = room->proceedMemberCount;
			auto& failed_ref = room->isGameReadyFailed;

			auto cnt = cnt_ref.Load(std::memory_order_acquire);
			auto failed = failed_ref.Load(std::memory_order_acquire);

			// just mark it as failed
			failed_ref.Store(true, std::memory_order_relaxed);
			failed = true;

			if (room->GetMembersCount() <= cnt + 1 or not room->CanPrepareGame())
			{
				if (room->TryCancelReady(iconer::app::RoomStates::Closing))
				{
					room->SetOperation(OpCloseGame);
					(void)Schedule(room, 0);
				}

				// rollback atomics
				cnt_ref.Store(0, std::memory_order_release);
				failed_ref.Store(false, std::memory_order_release);
			}
			else
			{
				// increase and release
				cnt_ref.CompareAndSet(cnt, cnt + 1, std::memory_order_release);
				// mark failed and release
				failed_ref.Store(true, std::memory_order_release);
			}
		}
		else
		{
			// rollback
			if (user.myRoomId.CompareAndSet(room_id, -1))
			{
				user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::Idle);
			}

			// cannot start a game: The client has a invalid room
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::InvalidRoom);
		}
	}
	else
	{
		// cannot start a game: The client is not in a room
		SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::NotInRoom);
	}
}

bool
demo::Framework::OnGameIsLoaded(iconer::app::User& user)
{
	using enum iconer::app::AsyncOperations;

	if (not user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::ReadyForGame))
	{
		// rollback everything
		const auto room_id = user.myRoomId.Load();

		if (room_id == -1)
		{
			user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::Idle);

			// cannot start a game: The client is not in a room
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::NotInRoom);
		}
		else if (auto room = FindRoom(room_id); room != nullptr)
		{
			auto& cnt_ref = room->proceedMemberCount;
			auto& failed_ref = room->isGameReadyFailed;

			auto cnt = cnt_ref.Load(std::memory_order_acquire);
			auto failed = failed_ref.Load(std::memory_order_acquire);

			// just mark it as failed
			failed_ref.Store(true, std::memory_order_relaxed);
			failed = true;

			// check condition using a plain field
			if (room->GetMembersCount() <= cnt or not room->CanPrepareGame())
			{
				if (room->TryCancelReady(iconer::app::RoomStates::Closing))
				{
					room->SetOperation(OpCloseGame);
					(void)Schedule(room, 0);
				}
				else
				{
					// cannot prepare the game: The operation is invalid
					SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::InvalidOperation);
				}

				// rollback atomics
				cnt_ref.Store(0, std::memory_order_release);
				failed_ref.Store(false, std::memory_order_release);
			}
			else
			{
				// increase and release
				cnt_ref.CompareAndSet(cnt, cnt + 1, std::memory_order_release);
				// mark failed and release
				failed_ref.Store(true, std::memory_order_release);
			}
		}
		else
		{
			// rollback
			if (user.myRoomId.CompareAndSet(room_id, -1))
			{
				user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::Idle);
			}

			// cannot start a game: The client has a invalid room
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::InvalidRoom);
		}
	}
	else
	{
		const auto room_id = user.myRoomId.Load();

		if (room_id == -1)
		{
			// rollback
			user.TryChangeState(iconer::app::UserStates::ReadyForGame, iconer::app::UserStates::Idle);
			user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::Idle);

			// cannot start a game: The client is not in a room
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::NotInRoom);
		}
		else if (auto room = FindRoom(room_id); room != nullptr)
		{
			auto& cnt_ref = room->proceedMemberCount;
			auto& failed_ref = room->isGameReadyFailed;

			auto cnt = cnt_ref.Load(std::memory_order_acquire);
			auto failed = failed_ref.Load(std::memory_order_acquire);

			if (failed)
			{
				// cannot start a game: Other clients has been failed
				SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::OtherClientFailed);
			}

			if (not room->CanPrepareGame() or not room->ReadyMember(user))
			{
				// mark failed at anytime
				failed_ref.Store(true, std::memory_order_relaxed);
				failed = true;

				// cannot start a game: The operation is invalid
				SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::InvalidOperation);
			}

			// check condition using plain fields
			if (room->GetMembersCount() <= cnt + 1)
			{
				using enum iconer::app::RoomStates;

				if (failed)
				{
					if (room->TryCancelReady(Closing))
					{
						room->SetOperation(OpCloseGame);
						(void)Schedule(room, 0);
					}
				}
				else if (room->TryMarkPreparedForGame())
				{
					// now let's start a game
					room->SetOperation(OpStartGame);
					if (not Schedule(room, 0))
					{
						if (room->TryUnmarkPreparedForGame(Closing))
						{
							room->SetOperation(OpCloseGame);
							(void)Schedule(room, 0);
						}
						else
						{
							room->TryUnmarkPreparedForGame(Idle);
						}
					}
				}
				else
				{
					if (room->TryCancelReady(Closing))
					{
						failed = true;

						room->SetOperation(OpCloseGame);
						(void)Schedule(room, 0);
					}
				}

				// rollback atomics
				cnt_ref.Store(0, std::memory_order_release);
				failed_ref.Store(false, std::memory_order_release);
			}
			else
			{
				// increase and release
				cnt_ref.CompareAndSet(cnt, cnt + 1, std::memory_order_release);
			}

			return not failed;
		}
		else
		{
			// rollback
			if (user.myRoomId.CompareAndSet(room_id, -1))
			{
				user.TryChangeState(iconer::app::UserStates::ReadyForGame, iconer::app::UserStates::Idle);
				user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::Idle);
			}

			// cannot start a game: The client has a invalid room
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::InvalidRoom);
		}
	}

	return false;
}

void
demo::Framework::OnFailedToLoadGame(iconer::app::User& user) noexcept
{
	if (const auto room_id = user.myRoomId.Load(); -1 != room_id)
	{
		if (auto room = FindRoom(room_id); nullptr != room)
		{
			auto& cnt_ref = room->proceedMemberCount;
			auto& failed_ref = room->isGameReadyFailed;

			auto cnt = cnt_ref.Load(std::memory_order_acquire);
			auto failed = failed_ref.Load(std::memory_order_acquire);

			// just mark it as failed
			failed_ref.Store(true, std::memory_order_relaxed);
			failed = true;

			if (room->GetMembersCount() <= cnt + 1 or not room->CanPrepareGame())
			{
				if (room->TryCancelReady(iconer::app::RoomStates::Closing))
				{
					room->SetOperation(OpCloseGame);
					(void)Schedule(room, 0);
				}

				// rollback atomics
				cnt_ref.Store(0, std::memory_order_release);
				failed_ref.Store(false, std::memory_order_release);
			}
			else
			{
				// increase and release
				cnt_ref.CompareAndSet(cnt, cnt + 1, std::memory_order_release);
				// mark failed and release
				failed_ref.Store(true, std::memory_order_release);
			}
		}
		else
		{
			// rollback
			if (user.myRoomId.CompareAndSet(room_id, -1))
			{
				user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::Idle);
			}

			// cannot start a game: The client has a invalid room
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::InvalidRoom);
		}
	}
	else
	{
		// cannot start a game: The client is not in a room
		SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::NotInRoom);
	}
}
