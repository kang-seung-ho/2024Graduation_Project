module;
#include <string_view>

#define SEND(user_var, method, ...) \
auto [io, ctx] = ((user_var).method)(__VA_ARGS__); \
if (not io) \
{ \
	ctx.Complete(); \
}

module Demo.Framework;
import Iconer.Application.RoomContract;
import Iconer.Application.GameContract;

bool
demo::Framework::OnGameIsLoaded(iconer::app::User& user)
{
	using namespace iconer::app;
	using namespace demo;
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
					room->gamePhaseTime = std::chrono::system_clock::now() + Room::gamePhasePeriod;

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
demo::Framework::OnFailedToLoadGame(iconer::app::User& user) noexcept
{
	using namespace iconer::app;
	using namespace demo;

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
