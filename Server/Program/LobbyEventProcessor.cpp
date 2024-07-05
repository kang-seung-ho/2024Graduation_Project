module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Framework;
import Iconer.App.User;
import Iconer.App.Room;
import Iconer.App.TaskContext;
import Iconer.App.PacketSerializer;

void
ServerFramework::EventOnMakeRoom(iconer::app::User& user, std::byte* data)
{
	using enum iconer::app::TaskCategory;

	auto& ctx = user.roomContext;

	if (ctx->TryChangeOperation(None, OpReserveRoom)) LIKELY
	{
		const auto ptr = roomManager.AcquireEmptyRoom(user);

		if (ptr.has_value()) LIKELY
		{
			const auto room = ptr.value();

			myTaskPool.Schedule(ctx, user.GetID(), static_cast<unsigned long>(room->GetID()));
		}
		else UNLIKELY
		{
			EventOnFailedToReserveRoom(user);
		};
	}
	else UNLIKELY
	{
		EventOnFailedToReserveRoom(user);
	};
}

void
ServerFramework::EventOnFailedToReserveRoom(iconer::app::User& user)
{
	using enum iconer::app::TaskCategory;

	auto& ctx = user.roomContext;
	ctx->TryChangeOperation(OpReserveRoom, None);
}

void
ServerFramework::EventOnFailedToMakeRoom(iconer::app::User& user)
{
	using enum iconer::app::TaskCategory;

	auto& ctx = user.roomContext;
	ctx->TryChangeOperation(OpCreateRoom, None);
}

void
ServerFramework::EventOnJoinRoom(iconer::app::User& user, std::byte* data)
{
	using enum iconer::app::TaskCategory;

	auto& ctx = user.roomContext;

	if (ctx->TryChangeOperation(None, OpEnterRoom)) LIKELY
	{
		std::int32_t room_id{};

		std::memcpy(&room_id, data, 4);

		if (0 < room_id) LIKELY
		{
			const auto room_seek = roomManager.FindRoom(static_cast<std::uintptr_t>(room_id));

			if (room_seek.has_value()) LIKELY
			{
				const auto room = room_seek.value();

				if (room->TryJoin(user))
				{
					user.SendRoomJoinedPacket(static_cast<std::uintptr_t>(room_id), user.GetID());
				}
				else
				{
					// rollback
					room->Leave(user);
					ctx->TryChangeOperation(OpEnterRoom, None);
				}
			}
			else
			{
				// rollback
				ctx->TryChangeOperation(OpEnterRoom, None);
			}
		}
		else UNLIKELY
		{
			// rollback
			ctx->TryChangeOperation(OpEnterRoom, None);
		}
	}
}

void
ServerFramework::EventOnFailedToJoinRoom(iconer::app::User& user)
{
	using enum iconer::app::TaskCategory;

	auto& ctx = user.roomContext;
	ctx->TryChangeOperation(OpEnterRoom, None);

	const auto room = user.myRoom.load();

	if (nullptr != room)
	{
		room->Leave(user);
	}
}

void
ServerFramework::EventOnExitRoom(iconer::app::User& user, std::byte* data)
{
	using enum iconer::app::TaskCategory;

	const auto room = user.myRoom.load();

	if (nullptr != room)
	{
		if (room->Leave(user))
		{
			// broadcast
		}
	}
}

void
ServerFramework::EventOnSeekRoom(iconer::app::User& user, std::byte* data)
{

}

void
ServerFramework::EventOnRoomList(iconer::app::User& user, std::byte* data)
{

}
