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

}

void
ServerFramework::EventOnExitRoom(iconer::app::User& user, std::byte* data)
{

}

void
ServerFramework::EventOnSeekRoom(iconer::app::User& user, std::byte* data)
{

}

void
ServerFramework::EventOnRoomList(iconer::app::User& user, std::byte* data)
{

}
