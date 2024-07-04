module;
module Iconer.App.RoomManager;
import Iconer.Net.ErrorCode;
import Iconer.Net.Socket;
import Iconer.Net.IoCompletionPort;
import Iconer.Net.IoContext;
import Iconer.App.User;
import Iconer.App.Room;
import <utility>;

void
iconer::app::RoomManager::Initialize()
{
	everyRooms.reserve(maxRoomCount);

	for (size_t i = 0; i < maxRoomCount; ++i)
	{
		iconer::app::Room* room = new iconer::app::Room{};

		AddRoom(room);
	}
}

void
iconer::app::RoomManager::Startup()
{}

void
iconer::app::RoomManager::Cleanup()
{
	for (auto& ptr : everyRooms)
	{
		delete std::exchange(ptr, nullptr);
	}
}

std::optional<iconer::app::RoomManager::pointer_type>
iconer::app::RoomManager::AcquireEmptyRoom(iconer::app::User& user)
const
{
	for (auto& ptr : everyRooms)
	{
		if (ptr->TryOccupy(user))
		{
			return ptr;
		}
	}

	return std::nullopt;
}

void
iconer::app::RoomManager::AddRoom(pointer_type session)
{
	everyRooms.emplace_back(session);
}
