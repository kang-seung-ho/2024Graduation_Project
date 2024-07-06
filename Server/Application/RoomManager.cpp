module;
#include <algorithm>

module Iconer.App.RoomManager;
import Iconer.Net.ErrorCode;
import Iconer.Net.IoCompletionPort;
import Iconer.Net.IoContext;
import Iconer.App.User;
import Iconer.App.Room;
import <utility>;

struct IdProjector
{
	[[nodiscard]]
	constexpr iconer::app::RoomManager::id_type operator()(const iconer::app::RoomManager::pointer_type& handle) const noexcept
	{
		return handle->GetID();
	}
};

void
iconer::app::RoomManager::Initialize()
{
	everyRooms.reserve(maxRoomCount);

	for (id_type id = minRoomUid; id < maxRoomUid; ++id)
	{
		iconer::app::Room* room = new iconer::app::Room{ id };

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

std::optional<iconer::app::RoomManager::pointer_type>
iconer::app::RoomManager::FindRoom(id_type id)
const noexcept
{
	auto it = std::ranges::lower_bound(everyRooms, id
		, std::less<id_type>{}
	, IdProjector{}
		);

	if (it != everyRooms.cend() and (*it)->GetID() == id)
	{
		return *it;
	}
	else
	{
		return std::nullopt;
	}
}

bool
iconer::app::RoomManager::HasRoom(id_type id)
const noexcept
{
	auto it = std::ranges::lower_bound(everyRooms, id
		, std::less<id_type>{}
	, IdProjector{}
		);

	return it != everyRooms.cend() and (*it)->GetID() == id;
}

void
iconer::app::RoomManager::AddRoom(pointer_type session)
{
	everyRooms.emplace_back(session);
}
