module;
#include <string_view>
#include <vector>
#include <atomic>

#define SEND(user_var, method, ...)\
auto [io, ctx] = ((user_var).method)(__VA_ARGS__);\
if (not io)\
{\
	ctx.Complete(); \
}
module Demo.Framework;

bool
demo::Framework::OnCreateGame(iconer::app::User& user)
{
	const auto room_id = user.myRoomId.Load();
	if (-1 == room_id)
	{
		// rollback
		user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::Idle);

		// cannot prepare the game: The client is not in a room
		SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::NotInRoom);

		return false;
	}
	else if (auto room = FindRoom(room_id); nullptr != room)
	{
		// make clients getting ready for game
		// for room master first
		if (not room->CanCreateGame())
		{
			// cannot prepare the game: The room is lack of members
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::LackOfMember);

			return false;
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

			return false;
		}
		else if (not room->HasMember(user.GetID()))
		{
			// rollback
			room->TryCancelReady();
			user.myRoomId.CompareAndSet(room_id, -1);
			user.TryChangeState(iconer::app::UserStates::MakingGame, iconer::app::UserStates::Idle);

			// cannot prepare the game: The client has no room
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::NotInRoom);

			return false;
		}
		else if (room->SetOperation(iconer::app::AsyncOperations::OpSpreadGameTicket);
			not Schedule(room, 0))
		{
			// rollback
			room->TryCancelReady();
			user.TryChangeState(iconer::app::UserStates::MakingGame, iconer::app::UserStates::InRoom);

			// cannot prepare the game: Server error occured
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::ServerError);

			return false;
		}
		else
		{
			return user.TryChangeState(iconer::app::UserStates::MakingGame, iconer::app::UserStates::InRoom);
		}
	}
	else
	{
		// rollback
		user.myRoomId = -1;
		user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::Idle);

		// cannot start a game: The client has a invalid room
		SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::InvalidRoom);

		return false;
	}
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

// TODO
bool
demo::Framework::OnBroadcastGameTickets(iconer::app::Room& room)
{
	if (room.CanPrepareGame())
	{
		std::vector<iconer::app::User*> members{ iconer::app::Room::maxUsersNumberInRoom };

		room.ForEach
		(
			[&members](iconer::app::User& member)
			{
				using enum iconer::app::UserStates;

				if (member.TryChangeState(InRoom, ReadyForGame))
				{
					members.push_back(std::addressof(member));
				}
			}
		);

		if (iconer::app::Room::minUsersNumberForGame <= members.size())
		{
			for (auto& member : members)
			{
				SEND(*member, SendMakeGameReadyPacket);
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

// TODO
void
demo::Framework::OnFailedToBroadcastGameTickets(iconer::app::Room& room)
noexcept
{
	room.TryCancelReady();
}

bool
demo::Framework::OnSentGameTicket(iconer::app::User& user)
{
	if (const auto room_id = user.myRoomId.Load(); -1 != room_id)
	{
		if (auto room = FindRoom(room_id); nullptr != room)
		{
			auto cnt = room->proceedMemberCount.Load(std::memory_order_acquire);
			auto failed = room->isGameReadyFailed.Load(std::memory_order_acquire);

			if (not room->HasMember(user.GetID()) or room->GetState() != iconer::app::RoomStates::Ready)
			{
				room->isGameReadyFailed.Store(true, std::memory_order_relaxed);
			}

			while (not room->proceedMemberCount.CompareAndSet(cnt, cnt + 1, std::memory_order_relaxed));

			if (room->GetMembersCount() <= cnt + 1)
			{
				if (failed or room->isGameReadyFailed.Load(std::memory_order_relaxed))
				{
					if (room->TryCancelReady(iconer::app::RoomStates::Closing))
					{
						room->SetOperation(iconer::app::AsyncOperations::OpCloseGame);
						(void)Schedule(room, 0);
					}
				}

				// rollback atomics
				room->proceedMemberCount.Store(0, std::memory_order_release);
				room->isGameReadyFailed.Store(false, std::memory_order_release);
			}

			return true;
		}
	}

	return false;
}

void
demo::Framework::OnFailedToSendGameTicket(iconer::app::User& user)
noexcept
{
	if (const auto room_id = user.myRoomId.Load(); -1 != room_id)
	{
		if (auto room = FindRoom(room_id); nullptr != room)
		{
			auto cnt = room->proceedMemberCount.Load(std::memory_order_acquire);
			room->isGameReadyFailed.Store(true, std::memory_order_release);

			while (not room->proceedMemberCount.CompareAndSet(cnt, cnt + 1, std::memory_order_relaxed));

			if (room->GetMembersCount() <= cnt + 1)
			{
				if (room->TryCancelReady(iconer::app::RoomStates::Closing))
				{
					room->SetOperation(iconer::app::AsyncOperations::OpCloseGame);
					(void)Schedule(room, 0);
				}

				// rollback atomics
				room->proceedMemberCount.Store(0, std::memory_order_release);
				room->isGameReadyFailed.Store(false, std::memory_order_release);
			}
		}
	}
}

// TODO
bool
demo::Framework::OnGameIsLoaded(iconer::app::User& user)
{
	const auto room_id = user.myRoomId.Load();
	if (room_id == -1)
	{
		// rollback
		user.TryChangeState(iconer::app::UserStates::MakingGame, iconer::app::UserStates::Idle);
		user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::Idle);

		return false;
	}
	else if (auto room = FindRoom(room_id); room != nullptr)
	{
		if (room->GetState() != iconer::app::RoomStates::Ready)
		{
			// cannot prepare the game: the room is unstable
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::UnstableRoom);

			return false;
		}
		else if (not room->isGameReadyFailed and room->ReadyMember(user))
		{
			return true;
		}
		else if (room->proceedMemberCount < room->GetMembersCount())
		{
			// mark room as failed
			room->isGameReadyFailed = true;

			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::OtherClientFailed);
		}
		else if (room->TryCancelReady(iconer::app::RoomStates::Closing))
		{
			room->SetOperation(iconer::app::AsyncOperations::OpCloseGame);
			(void)Schedule(room, 0);
		}
		else
		{
			SEND(user, SendCannotStartGamePacket, iconer::app::GameContract::UnstableRoom);
		}
	}
	else
	{
		// rollback (failsafe)
		if (user.myRoomId.CompareAndSet(room_id, -1))
		{
			user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::Idle);
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
			if (room->TryCancelReady(iconer::app::RoomStates::Closing))
			{
				room->SetOperation(iconer::app::AsyncOperations::OpCloseGame);
				(void)Schedule(room, 0);
			}
		}
	}
}

bool
demo::Framework::OnStartGame(iconer::app::User& user, iconer::app::Room& room)
{
	return true;
}

void
demo::Framework::OnFailedToStartGame(iconer::app::User& user)
noexcept
{}

void
demo::Framework::OnCloseGame(iconer::app::Room& room)
{
	if (room.TryEndClose(0 < room.GetMembersCount()
		? iconer::app::RoomStates::Idle : iconer::app::RoomStates::None))
	{
		room.SetOperation(iconer::app::AsyncOperations::None);
		room.isGameReadyFailed = false;

		room.ForEach
		(
			[](iconer::app::User& member)
			{
				member.TryChangeState(iconer::app::UserStates::MakingGame, iconer::app::UserStates::InRoom);
				member.TryChangeState(iconer::app::UserStates::ReadyForGame, iconer::app::UserStates::InRoom);
			}
		);
	}
}
