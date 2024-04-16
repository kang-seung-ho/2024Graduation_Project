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

using namespace iconer::app;

bool
demo::Framework::OnCreateGame(User& user)
{
	using enum AsyncOperations;

	const auto room_id = user.myRoomId.Load();
	if (-1 == room_id)
	{
		// rollback
		user.TryChangeState(UserStates::InRoom, UserStates::Idle);

		// cannot prepare the game: The client is not in a room
		SEND(user, SendCannotStartGamePacket, GameContract::NotInRoom);
	}
	else if (auto room = FindRoom(room_id); nullptr != room)
	{
		// make clients getting ready for game
		// for room master first
		if (not room->CanCreateGame())
		{
			// cannot prepare the game: The room is lack of member
			SEND(user, SendCannotStartGamePacket, GameContract::LackOfMember);
		}
		else if (not user.TryChangeState(UserStates::InRoom, UserStates::MakingGame))
		{
			// cannot prepare the game: The client is busy
			SEND(user, SendCannotStartGamePacket, GameContract::ClientIsBusy);

		}
		else if (not room->TryGettingReady())
		{
			// rollback
			user.TryChangeState(UserStates::MakingGame, UserStates::InRoom);

			// cannot prepare the game: Not proper operation
			SEND(user, SendCannotStartGamePacket, GameContract::InvalidOperation);
		}
		else if (not room->HasMember(user.GetID()))
		{
			// rollback
			user.myRoomId.CompareAndSet(room_id, -1);
			user.TryChangeState(UserStates::MakingGame, UserStates::Idle);
			room->TryCancelReady();

			// cannot prepare the game: The client has no room
			SEND(user, SendCannotStartGamePacket, GameContract::NotInRoom);
		}
		else if (room->SetOperation(OpSpreadGameTicket);
			not Schedule(room, 0))
		{
			// rollback
			user.TryChangeState(UserStates::MakingGame, UserStates::InRoom);
			room->TryCancelReady();

			// cannot prepare the game: Server error occured
			SEND(user, SendCannotStartGamePacket, GameContract::ServerError);
		}
		else
		{
			return user.TryChangeState(UserStates::MakingGame, UserStates::InRoom);
		}
	}
	else
	{
		// rollback
		if (user.myRoomId.CompareAndSet(room_id, -1))
		{
			user.TryChangeState(UserStates::InRoom, UserStates::Idle);
		}

		// cannot start a game: The client has a invalid room
		SEND(user, SendCannotStartGamePacket, GameContract::InvalidRoom);
	}

	return false;
}

void
demo::Framework::OnFailedToCreateGame(User& user)
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
demo::Framework::OnBroadcastGameTickets(Room& room)
{
	if (room.CanPrepareGame())
	{
		std::vector<User*> members{};
		members.reserve(Room::maxUsersNumberInRoom);

		room.ForEach
		(
			[&members](User& member)
			{
				members.push_back(std::addressof(member));
			}
		);

#define MUST_START true

#if MUST_START
		if (0 < members.size())
#else
		if (Room::minUsersNumberForGame <= members.size())
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
				SEND(*member, SendCannotStartGamePacket, GameContract::LackOfMember);
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
demo::Framework::OnFailedToBroadcastGameTickets(Room& room)
noexcept
{
	room.TryCancelReady();
}

bool
demo::Framework::OnSentGameTicket(User& user)
{
	using enum AsyncOperations;

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
				SEND(user, SendCannotStartGamePacket, GameContract::OtherClientFailed);
			}

			if (not room->HasMember(user.GetID()) or not room->CanPrepareGame())
			{
				// mark failed at anytime
				failed_ref.Store(true, std::memory_order_relaxed);
				failed = true;

				// cannot start a game: The room is unstable
				SEND(user, SendCannotStartGamePacket, GameContract::UnstableRoom);
			}

			// check condition using plain fields
			if (room->GetMembersCount() <= cnt + 1)
			{
				if (failed)
				{
					if (room->TryCancelReady(RoomStates::Closing))
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
				user.TryChangeState(UserStates::InRoom, UserStates::Idle);
			}

			// cannot start a game: The client has a invalid room
			SEND(user, SendCannotStartGamePacket, GameContract::InvalidRoom);
		}
	}
	else
	{
		// cannot start a game: The client is not in a room
		SEND(user, SendCannotStartGamePacket, GameContract::NotInRoom);
	}

	return false;
}

void
demo::Framework::OnFailedToSendGameTicket(User& user)
noexcept
{
	using enum AsyncOperations;

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
				if (room->TryCancelReady(RoomStates::Closing))
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
				user.TryChangeState(UserStates::InRoom, UserStates::Idle);
			}

			// cannot start a game: The client has a invalid room
			SEND(user, SendCannotStartGamePacket, GameContract::InvalidRoom);
		}
	}
	else
	{
		// cannot start a game: The client is not in a room
		SEND(user, SendCannotStartGamePacket, GameContract::NotInRoom);
	}
}

bool
demo::Framework::OnGameIsLoaded(User& user)
{
	using enum AsyncOperations;

	if (not user.TryChangeState(UserStates::InRoom, UserStates::ReadyForGame))
	{
		// rollback everything
		const auto room_id = user.myRoomId.Load();

		if (room_id == -1)
		{
			user.TryChangeState(UserStates::InRoom, UserStates::Idle);

			// cannot start a game: The client is not in a room
			SEND(user, SendCannotStartGamePacket, GameContract::NotInRoom);
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
				if (room->TryCancelReady(RoomStates::Closing))
				{
					room->SetOperation(OpCloseGame);
					(void)Schedule(room, 0);
				}
				else
				{
					// cannot prepare the game: The operation is invalid
					SEND(user, SendCannotStartGamePacket, GameContract::InvalidOperation);
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
				user.TryChangeState(UserStates::InRoom, UserStates::Idle);
			}

			// cannot start a game: The client has a invalid room
			SEND(user, SendCannotStartGamePacket, GameContract::InvalidRoom);
		}
	}
	else
	{
		const auto room_id = user.myRoomId.Load();

		if (room_id == -1)
		{
			// rollback
			user.TryChangeState(UserStates::ReadyForGame, UserStates::Idle);
			user.TryChangeState(UserStates::InRoom, UserStates::Idle);

			// cannot start a game: The client is not in a room
			SEND(user, SendCannotStartGamePacket, GameContract::NotInRoom);
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
				SEND(user, SendCannotStartGamePacket, GameContract::OtherClientFailed);
			}

			if (not room->CanPrepareGame() or not room->ReadyMember(user))
			{
				// mark failed at anytime
				failed_ref.Store(true, std::memory_order_relaxed);
				failed = true;

				// cannot start a game: The operation is invalid
				SEND(user, SendCannotStartGamePacket, GameContract::InvalidOperation);
			}

			// check condition using plain fields
			if (room->GetMembersCount() <= cnt + 1)
			{
				using enum RoomStates;

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
				user.TryChangeState(UserStates::ReadyForGame, UserStates::Idle);
				user.TryChangeState(UserStates::InRoom, UserStates::Idle);
			}

			// cannot start a game: The client has a invalid room
			SEND(user, SendCannotStartGamePacket, GameContract::InvalidRoom);
		}
	}

	return false;
}

void
demo::Framework::OnFailedToLoadGame(User& user) noexcept
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
				if (room->TryCancelReady(RoomStates::Closing))
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
				user.TryChangeState(UserStates::InRoom, UserStates::Idle);
			}

			// cannot start a game: The client has a invalid room
			SEND(user, SendCannotStartGamePacket, GameContract::InvalidRoom);
		}
	}
	else
	{
		// cannot start a game: The client is not in a room
		SEND(user, SendCannotStartGamePacket, GameContract::NotInRoom);
	}
}
