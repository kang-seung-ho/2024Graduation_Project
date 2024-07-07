module;
#include <algorithm>

module Iconer.App.RoomManager;
import iconer.Utility.Serializer;
import Iconer.App.User;
import Iconer.App.Room;
import Iconer.App.PacketSerializer;
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

	using iconer::util::MakeInvoker;
	using iconer::util::MakeSharedInvoker;

	for (id_type id = minRoomUid; id < maxRoomUid; ++id)
	{
		iconer::app::Room* room = new iconer::app::Room{ id };

		room->onOccupied.Add(MakeInvoker(this, &RoomManager::OnRoomMade));
		room->onUserJoined.Add(MakeInvoker(this, &RoomManager::OnUserJoined));
		room->onUserLeft.Add(MakeInvoker(this, &RoomManager::OnUserLeft));
		room->onDestroyed.Add(MakeInvoker(this, &RoomManager::OnRoomClosed));

		room->Initialize();

		AddRoom(room);
	}

	isRoomDirty = true;

	(void)iconer::app::SerializeAt(precachedRoomListData, PacketProtocol::SC_RESPOND_ROOMS, 0ULL);
	precachedRoomListDataSize = 3 + sizeof(size_t);
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
iconer::app::RoomManager::AcquireEmptyRoom(iconer::app::RoomManager::value_type& user)
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

std::span<const std::byte>
iconer::app::RoomManager::AcquireCachedRoomData()
{
	const auto is_dirty = isRoomDirty.load(std::memory_order_acquire);

	if (is_dirty)
	{
		auto seek = precachedRoomListData.data() + 3 + sizeof(size_t);

		// Serialize rooms
		size_t count{};

		for (auto& room : everyRooms)
		{
			if (not room->IsTaken()) continue;

			seek = iconer::util::Serialize(seek, static_cast<std::int32_t>(room->GetID()));

			constexpr auto len = iconer::app::Settings::roomTitleLength;

			wchar_t serialized_title[len]{};
			const auto title = room->GetTitle().substr(0, len);

			std::copy(title.cbegin(), title.cend(), serialized_title);

			seek = iconer::util::Serialize(seek, serialized_title);
			seek = iconer::util::Serialize(seek, room->GetMemberCount());

			++count;
		}

		precachedRoomListDataSize = 3 + sizeof(size_t) + SerializedRoom::GetSerializedSize() * count;

		auto post = iconer::util::Serialize(precachedRoomListData.data() + 1, static_cast<std::int16_t>(precachedRoomListDataSize));
		iconer::util::Serialize(post, count);

		isRoomDirty.store(false, std::memory_order_release);
	}

	return std::span<const std::byte>{ precachedRoomListData.data(), precachedRoomListDataSize };
}

std::optional<iconer::app::RoomManager::pointer_type>
iconer::app::RoomManager::FindRoom(iconer::app::RoomManager::id_type id)
const noexcept
{
	auto it = std::ranges::lower_bound(everyRooms, id
		, std::less<id_type>{}, IdProjector{});

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
		, std::less<id_type>{}, IdProjector{});

	return it != everyRooms.cend() and (*it)->GetID() == id;
}

void
iconer::app::RoomManager::AddRoom(pointer_type session)
{
	everyRooms.emplace_back(session);
}

void
iconer::app::RoomManager::OnRoomMade(iconer::app::RoomManager::pointer_type room, iconer::app::RoomManager::value_type* user)
{
	isRoomDirty.store(true, std::memory_order_release);
}

void
iconer::app::RoomManager::OnUserJoined(iconer::app::RoomManager::pointer_type room, iconer::app::RoomManager::value_type* user, std::int32_t member_cnt)
{
	isRoomDirty.store(true, std::memory_order_release);
}

void
iconer::app::RoomManager::OnUserLeft(iconer::app::RoomManager::pointer_type room, iconer::app::RoomManager::value_type* user, std::int32_t member_cnt)
{
	isRoomDirty.store(true, std::memory_order_release);
}

void
iconer::app::RoomManager::OnRoomClosed(iconer::app::RoomManager::pointer_type room)
{
	isRoomDirty.store(true, std::memory_order_release);
}
