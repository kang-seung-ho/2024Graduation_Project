module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Framework;
import Iconer.App.User;
import Iconer.App.Room;
import Iconer.App.SendContext;
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

			constexpr auto len = iconer::app::Settings::roomTitleLength;

			wchar_t title[len + 1]{};

			std::memcpy(title, data, len * 2);

			room->SetTitle(title);

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
					const auto sender = AcquireSendContext();
					auto [pk, size] = room->MakeMemberJoinedPacket(user);
					
					const auto data_ptr = pk.get();
					sender->myBuffer = std::move(pk);
					sender->SetOperation(OpEnterRoom);

					user.SendGeneralData(*sender, size);
				}
				else
				{
					// rollback
					room->Leave(user, false);
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
ServerFramework::EventOnNotifyRoomJoin(iconer::app::User& user)
{
	using enum iconer::app::PacketProtocol;
	using enum iconer::app::TaskCategory;

	auto& ctx = user.roomContext;

	const auto room = user.myRoom.load();

	if (nullptr != room)
	{
		constexpr auto len = iconer::app::Settings::nickNameLength;

		wchar_t nickname_buf[len]{};
		const auto nickname = user.GetName().substr(0, len);
		std::copy(nickname.cbegin(), nickname.cend(), nickname_buf);

		const auto id = static_cast<std::int32_t>(user.GetID());
		const auto team = user.GetID() % 2 == 0 ? (char)1 : (char)2;
		const auto rid = static_cast<std::int32_t>(room->GetID());

		// broadcast
		room->Foreach([&](iconer::app::User& mem)
			{
				if (mem.GetID() == user.GetID()) return;

				auto sender = AcquireSendContext();

				auto [pk, length] = iconer::app::Serialize(SC_ROOM_JOINED, id, team, nickname_buf, rid);

				sender->myBuffer = std::move(pk);

				mem.SendGeneralData(*sender, static_cast<size_t>(length));
			}
		);
	}

	ctx->TryChangeOperation(OpEnterRoom, None);
}

void
ServerFramework::EventOnFailedToNotifyRoomJoin(iconer::app::User& user)
{
	using enum iconer::app::TaskCategory;

	auto& ctx = user.roomContext;
	ctx->TryChangeOperation(OpEnterRoom, None);

	const auto room = user.myRoom.load();

	if (nullptr != room)
	{
		room->Leave(user, false);
	}
}

void
ServerFramework::EventOnExitRoom(iconer::app::User& user, std::byte* data)
{
	using enum iconer::app::PacketProtocol;
	using enum iconer::app::TaskCategory;

	const auto room = user.myRoom.load();

	if (nullptr != room)
	{
		if (room->Leave(user))
		{
			// broadcast
			room->Foreach([&, id = static_cast<std::int32_t>(user.GetID())](iconer::app::User& mem)
				{
					if (mem.GetID() == user.GetID()) return;

					auto ctx = AcquireSendContext();

					auto [pk, length] = iconer::app::Serialize(SC_ROOM_LEFT, id);

					ctx->myBuffer = std::move(pk);

					mem.SendGeneralData(*ctx, static_cast<size_t>(length));
				}
			);
		}
	}
}

void
ServerFramework::EventOnSeekRoom(iconer::app::User& user, std::byte* data)
{

}

void
ServerFramework::EventOnRoomList(iconer::app::User& user, std::byte*)
{
	user.SendGeneralData(*AcquireSendContext(), roomManager.AcquireCachedRoomData());
}
