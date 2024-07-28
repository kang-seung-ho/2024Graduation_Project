module;
#include <string_view>

module Demo.Framework;
import Iconer.Application.RoomContract;

bool RemoveRoomMember(demo::Framework& framework, iconer::app::Room& room, const iconer::app::User::IdType user_id) noexcept;

iconer::app::RoomContract
demo::Framework::OnReservingRoom(iconer::app::Room& room, iconer::app::User& user)
{
	if (not room.TryBeginCreate())
	{
		// the room is busy
		return iconer::app::RoomContract::RoomIsBusy;
	}
	else
	{
		const auto& room_id = room.GetID();
		if (not user.myRoomId.CompareAndSet(-1, room_id))
		{
			// rollback
			room.TryCancelCreating();

			// room is already assigned to the client
			return iconer::app::RoomContract::AnotherRoomIsAlreadyAssigned;
		}
		else if (not room.TryAddMember(user))
		{
			// rollback
			room.TryCancelCreating();
			user.myRoomId.CompareAndSet(room_id, -1);

			// the room is full
			return iconer::app::RoomContract::RoomIsFull;
		}

		room.SetOperation(iconer::app::AsyncOperations::OpCreateRoom);

		auto sent_r = user.SendRoomCreatedPacket(std::addressof(room), room_id);
		if (not sent_r)
		{
			// rollback
			if (room.TryCancelCreating())
			{
				room.SetOperation(iconer::app::AsyncOperations::None);
			}
			::RemoveRoomMember(*this, room, user.GetID());

			user.myRoomId.CompareAndSet(room_id, -1);

			// failed to notify
			return iconer::app::RoomContract::ServerError;
		}
	}

	SetRoomModifiedFlag();
	return iconer::app::RoomContract::Success;
}

void
demo::Framework::OnFailedToReserveRoom(iconer::app::Room& room, iconer::app::User& user, iconer::app::RoomContract reason)
{
	if (room.TryCancelCreating())
	{
		room.SetOperation(iconer::app::AsyncOperations::None);
	}
	else
	{
		room.TryCancelContract();
	}

	::RemoveRoomMember(*this, room, user.GetID());
	user.myRoomId.CompareAndSet(room.GetID(), -1);

	auto [io, ctx] = user.SendRoomCreationFailedPacket(reason);
	if (not io)
	{
		ctx.Complete();
	}
}

iconer::app::RoomContract
demo::Framework::OnCreatingRoom(iconer::app::Room& room, iconer::app::User& user)
{
	if (not room.TryEstablish())
	{
		// room is unstable
		return iconer::app::RoomContract::UnstableRoom;
	}
	else if (not user.TryChangeState(iconer::app::UserStates::Idle, iconer::app::UserStates::InRoom))
	{
		return iconer::app::RoomContract::InvalidOperation;
	}
	else
	{
		room.SetOperation(iconer::app::AsyncOperations::None);
	}

	return iconer::app::RoomContract::Success;
}

void
demo::Framework::OnFailedToCreateRoom(iconer::app::Room& room, iconer::app::User& user, iconer::app::RoomContract reason)
{
	if (room.TryCancelCreating())
	{
		room.SetOperation(iconer::app::AsyncOperations::None);
	}

	::RemoveRoomMember(*this, room, user.GetID());
	user.myRoomId.CompareAndSet(room.GetID(), -1);

	auto [io, ctx] = user.SendRoomCreationFailedPacket(reason);
	if (not io)
	{
		ctx.Complete();
	}
}

iconer::app::RoomContract
demo::Framework::OnJoiningRoom(iconer::app::Room& room, iconer::app::User& user)
{
	if (room.IsFull())
	{
		// room is full
		return iconer::app::RoomContract::RoomIsFull;
	}
	else if (room.GetState() != iconer::app::RoomStates::Idle)
	{
		// rollback
		user.TryChangeState(iconer::app::UserStates::EnteringRoom, iconer::app::UserStates::Idle);

		// room is busy
		return iconer::app::RoomContract::RoomIsBusy;
	}
	else if (not user.TryChangeState(iconer::app::UserStates::EnteringRoom, iconer::app::UserStates::EnteringRoom))
	{
		// rollback
		user.TryChangeState(iconer::app::UserStates::EnteringRoom, iconer::app::UserStates::Idle);

		return iconer::app::RoomContract::InvalidOperation;
	}
	else
	{
		const auto& room_id = room.GetID();
		if (not user.myRoomId.CompareAndSet(-1, room_id))
		{
			// rollback
			user.TryChangeState(iconer::app::UserStates::EnteringRoom, iconer::app::UserStates::Idle);

			// another room is already assigned to the client
			return iconer::app::RoomContract::AnotherRoomIsAlreadyAssigned;
		}
		else if (not room.TryAddMember(user))
		{
			// rollback
			user.TryChangeState(iconer::app::UserStates::EnteringRoom, iconer::app::UserStates::Idle);
			user.myRoomId.CompareAndSet(room_id, -1);

			// the room is full
			return iconer::app::RoomContract::RoomIsFull;
		}
		else if (not user.TryChangeState(iconer::app::UserStates::EnteringRoom, iconer::app::UserStates::InRoom))
		{
			// rollback
			user.TryChangeState(iconer::app::UserStates::EnteringRoom, iconer::app::UserStates::Idle);
			user.myRoomId.CompareAndSet(room_id, -1);
			::RemoveRoomMember(*this, room, user.GetID());

			return iconer::app::RoomContract::InvalidOperation;
		}

		auto sent_r = user.SendRoomJoinedPacket(room_id, user);
		if (not sent_r.first.has_value())
		{
			// rollback
			user.TryChangeState(iconer::app::UserStates::EnteringRoom, iconer::app::UserStates::Idle);
			user.myRoomId.CompareAndSet(room_id, -1);
			::RemoveRoomMember(*this, room, user.GetID());

			return iconer::app::RoomContract::ServerError;
		}

		return iconer::app::RoomContract::Success;
	}
}

void
demo::Framework::OnFailedToJoinRoom(iconer::app::Room& room, iconer::app::User& user, iconer::app::RoomContract reason)
{
	// rollback
	user.TryChangeState(iconer::app::UserStates::EnteringRoom, iconer::app::UserStates::Idle);
	user.myRoomId.CompareAndSet(room.GetID(), -1);

	::RemoveRoomMember(*this, room, user.GetID());

	auto [io, ctx] = user.SendRoomJoinFailedPacket(reason);
	if (not io)
	{
		ctx.Complete();
	}
}

void
demo::Framework::OnSucceedToJoinRoom(iconer::app::Room& room, iconer::app::User& user)
{
	room.ForEach([&user, user_id = user.GetID(), room_id = room.GetID()](iconer::app::User& member)
		{
			if (member.GetID() != user_id)
			{
				auto [io, ctx] = member.SendRoomJoinedPacket(room_id, user);
				if (not io)
				{
					ctx.Complete();
				}
			}
		}
	);
}

bool
demo::Framework::OnLeavingRoom(iconer::app::User& user)
{
	if (auto room_id = user.myRoomId.Exchange(-1); -1 != room_id)
	{
		user.TryChangeState(iconer::app::UserStates::InRoom, iconer::app::UserStates::Idle);

		if (auto room = FindRoom(room_id); nullptr != room)
		{
			return ::RemoveRoomMember(*this, *room, user.GetID());
		}
	}

	return false;
}

void
demo::Framework::OnClosingRoom(iconer::app::Room& room)
{
	room.Cleanup();
}

void
demo::Framework::OnFailedReceive(iconer::app::User& user)
{
	struct ReceiveRemover final : public iconer::app::Room::MemberRemover
	{
		using Super = iconer::app::Room::MemberRemover;

		constexpr ReceiveRemover(demo::Framework& framework) noexcept
			: Super(), myFramework(framework)
		{}

		void operator()(volatile iconer::app::Room& room, const size_t& members_count) const noexcept override
		{
			if (0 == members_count)
			{
				room.BeginClose();
				room.SetOperation(iconer::app::AsyncOperations::OpCloseRoom);

				if (not myFramework.Schedule(room, room.GetID()))
				{
					room.Cleanup();
				}
			}
		}

		demo::Framework& myFramework;
	};

	// Make room out now
	if (auto room_id = user.myRoomId.Exchange(-1); -1 != room_id)
	{
		if (auto room = FindRoom(room_id); nullptr != room)
		{
			if (bool removed = room->RemoveMember(user.GetID(), ReceiveRemover{ *this }); removed)
			{
				SetRoomModifiedFlag();

				room->ForEach([user_id = user.GetID()](iconer::app::User& member)
					{
						if (&member == nullptr) return;

						if (member.IsOnline())
						{
							if (member.GetID() != user_id) // `user` already have sent a packet
							{
								// just send the packet
								auto [io, ctx] = member.SendRoomLeftPacket(user_id, false);
								if (not io)
								{
									ctx.Complete();
								}
							}
						}
					}
				);
			}
		}
	}

	user.Cleanup();
	user.BeginClose();
}

bool
RemoveRoomMember(demo::Framework& framework, iconer::app::Room& room, const iconer::app::User::IdType user_id)
noexcept
{
	struct Remover final : public iconer::app::Room::MemberRemover
	{
		using Super = iconer::app::Room::MemberRemover;

		constexpr Remover(demo::Framework& framework) noexcept
			: Super(), myFramework(framework)
		{}

		void operator()(volatile iconer::app::Room& room, const size_t& members_count) const noexcept override
		{
			if (0 == members_count)
			{
				if (room.TryBeginClose(iconer::app::RoomStates::Idle))
				{
					room.SetOperation(iconer::app::AsyncOperations::OpCloseRoom);

					if (not myFramework.Schedule(room, room.GetID()))
					{
						room.Cleanup();
					}
				}
			}
		}

		demo::Framework& myFramework;
	};

	if (auto removed = room.RemoveMember(user_id, Remover{ framework }); removed)
	{
		framework.SetRoomModifiedFlag();

		room.ForEach([user_id](iconer::app::User& member)
			{
				if (member.IsOnline())
				{
					if (member.GetID() != user_id) // `user` already have sent a packet
					{
						// just send the packet
						auto [io, ctx] = member.SendRoomLeftPacket(user_id, false);
						if (not io)
						{
							ctx.Complete();
						}
					}
				}
			}
		);

		return true;
	}
	else
	{
		return false;
	}
}
