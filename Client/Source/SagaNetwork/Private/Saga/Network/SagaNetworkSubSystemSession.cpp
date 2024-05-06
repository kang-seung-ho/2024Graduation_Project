#include "Saga/Network/SagaNetworkSubSystem.h"

void
USagaNetworkSubSystem::AddUser(const FSagaVirtualUser& client)
{
	everyUsers.Add(client);
	wasUsersUpdated = true;
}

bool
USagaNetworkSubSystem::RemoveUser(int32 id)
noexcept
{
	const bool result = 0 < everyUsers.RemoveAllSwap(FSagaSessionIdComparator{ id });
	if (result)
	{
		wasUsersUpdated = true;
	}

	return result;
}

void
USagaNetworkSubSystem::ClearUserList()
noexcept
{
	everyUsers.Reset();
	wasUsersUpdated = true;
}

const TArray<FSagaVirtualUser>&
USagaNetworkSubSystem::GetUserList()
const noexcept
{
	return everyUsers;
}

bool
USagaNetworkSubSystem::FindUser(int32 id, FSagaVirtualUser& outpin)
const noexcept
{
	auto handle = everyUsers.FindByPredicate(FSagaSessionIdComparator{ id });
	if (nullptr != handle)
	{
		outpin = *handle;
		return true;
	}
	else
	{
		return false;
	}
}

bool
USagaNetworkSubSystem::HasUser(int32 id)
const noexcept
{
	return everyUsers.ContainsByPredicate(FSagaSessionIdComparator{ id });
}

bool
USagaNetworkSubSystem::GetTeam(int32 user_id, EUserTeam& outpin)
const noexcept
{
	FSagaVirtualUser user{};

	if (FindUser(user_id, user))
	{
		outpin = user.myTeam;
		return true;
	}
	else
	{
		return false;
	}
}

void
USagaNetworkSubSystem::AddRoom(const FSagaVirtualRoom& room)
{
	everyRooms.Add(room);
	wasRoomsUpdated = true;
}

bool
USagaNetworkSubSystem::RemoveRoom(int32 id)
noexcept
{
	const bool result = 0 < everyRooms.RemoveAllSwap(FSagaSessionIdComparator{ id });
	if (result)
	{
		wasRoomsUpdated = true;
	}

	return result;
}

void
USagaNetworkSubSystem::ClearRoomList()
noexcept
{
	everyRooms.Reset();
	wasRoomsUpdated = true;
}

const TArray<FSagaVirtualRoom>&
USagaNetworkSubSystem::GetRoomList()
const noexcept
{
	return everyRooms;
}

bool
USagaNetworkSubSystem::RoomAt(int32 index, FSagaVirtualRoom& outpin)
noexcept
{
	if (everyRooms.IsValidIndex(index))
	{
		outpin = everyRooms[index];
		return true;
	}
	else
	{
		return false;
	}
}

bool
USagaNetworkSubSystem::FindRoom(int32 id, FSagaVirtualRoom& outpin)
const noexcept
{
	auto handle = everyRooms.FindByPredicate(FSagaSessionIdComparator{ id });
	if (nullptr != handle)
	{
		outpin = *handle;
		return true;
	}
	else
	{
		return false;
	}
}

bool
USagaNetworkSubSystem::HasRoom(int32 id)
const noexcept
{
	return everyRooms.ContainsByPredicate(FSagaSessionIdComparator{ id });
}

void
USagaNetworkSubSystem::SetLocalUserId(int32 id)
noexcept
{
	localUserId = id;
}

int32
USagaNetworkSubSystem::GetLocalUserId()
const noexcept
{
	return localUserId;
}

void
USagaNetworkSubSystem::SetLocalUserName(const FString& nickname)
{
	localUserName = nickname;
}

FString
USagaNetworkSubSystem::GetLocalUserName()
const
{
	return localUserName;
}

bool
USagaNetworkSubSystem::GetLocalUserTeam(EUserTeam& outpin)
const noexcept
{
	return GetTeam(localUserId, outpin);
}

void
USagaNetworkSubSystem::SetCurrentRoomId(int32 id)
noexcept
{
	currentRoomId = id;
}

int32
USagaNetworkSubSystem::GetCurrentRoomId()
const noexcept
{
	return currentRoomId;
}

void
USagaNetworkSubSystem::SetCurrentRoomTitle(const FString& title)
{
	currentRoomTitle = title;
}

FString
USagaNetworkSubSystem::GetCurrentRoomTitle()
const
{
	return currentRoomTitle;
}
