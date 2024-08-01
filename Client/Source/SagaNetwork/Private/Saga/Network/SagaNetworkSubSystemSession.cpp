#include "Saga/Network/SagaNetworkSubSystem.h"

void
USagaNetworkSubSystem::AddUser(const FSagaVirtualUser& client)
{
	everyUsers.Add(client);
	wasUsersUpdated = true;
}

void
USagaNetworkSubSystem::AddUser(FSagaVirtualUser&& client)
{
	everyUsers.Add(MoveTemp(client));
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
USagaNetworkSubSystem::ClearUserList(bool removed_me_also)
noexcept
{
	if (removed_me_also)
	{
		//localUser = {};
		everyUsers.Reset();
	}
	else
	{
		everyUsers.RemoveAllSwap(
			[my_id = GetLocalUserId()](const FSagaVirtualUser& user) -> bool
			{
				return user.myID != my_id;
			}
		);
	}

	wasUsersUpdated = true;
}

const TArray<FSagaVirtualUser>&
USagaNetworkSubSystem::GetUserList()
const noexcept
{
	return everyUsers;
}

bool
USagaNetworkSubSystem::FindUser(int32 user_id, FSagaVirtualUser& outpin)
const noexcept
{
	if (IsOfflineMode())
	{
		outpin = localUser;

		return true;
	}

	auto handle = everyUsers.FindByPredicate(FSagaSessionIdComparator{ user_id });
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
USagaNetworkSubSystem::HasUser(int32 user_id)
const noexcept
{
	if (user_id == GetLocalUserId())
	{
		return true;
	}

	return everyUsers.ContainsByPredicate(FSagaSessionIdComparator{ user_id });
}

void
USagaNetworkSubSystem::SetCharacterHandle(int32 user_id, ASagaCharacterBase* character)
noexcept
{
	if (user_id == GetLocalUserId())
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("[USagaNetworkSubSystem] Local user '%d's character is set."), user_id);

		localUser.SetCharacterHandle(character);
	}

	for (auto& user : everyUsers)
	{
		if (user.myID == user_id)
		{
			UE_LOG(LogSagaNetwork, Log, TEXT("[USagaNetworkSubSystem] User %d's character is set."), user_id);

			user.remoteCharacter = character;

			return;
		}
	}
}

void
USagaNetworkSubSystem::SetTeam(int32 user_id, const ESagaPlayerTeam& team)
noexcept
{
	if (user_id == GetLocalUserId())
	{
		localUser.myTeam = team;
	}

	for (auto& user : everyUsers)
	{
		if (user.myID == user_id)
		{
			user.myTeam = team;
		}
	}
}

void
USagaNetworkSubSystem::SetWeapon(int32 user_id, EPlayerWeapon weapon)
noexcept
{
	if (user_id == GetLocalUserId())
	{
		localUser.myWeapon = weapon;
	}

	for (auto& user : everyUsers)
	{
		if (user.myID == user_id)
		{
			user.myWeapon = weapon;
		}
	}
}

void
USagaNetworkSubSystem::SetHealth(int32 user_id, const float hp)
noexcept
{
	if (user_id == GetLocalUserId())
	{
		localUser.myHealth = hp;
	}

	for (auto& user : everyUsers)
	{
		if (user.myID == user_id)
		{
			user.myHealth = hp;
		}
	}
}

void
USagaNetworkSubSystem::StorePosition(int32 user_id, const double& x, const double& y, const double& z)
noexcept
{
	if (user_id == GetLocalUserId())
	{
		localUser.myX = x;
		localUser.myY = y;
		localUser.myZ = z;
	}

	for (auto& user : everyUsers)
	{
		if (user.myID == user_id)
		{
			user.myX = x;
			user.myY = y;
			user.myZ = z;
		}
	}
}

ASagaCharacterBase*
USagaNetworkSubSystem::GetCharacterHandle(int32 user_id)
const noexcept
{
	if (IsOfflineMode())
	{
		return localUser.GetCharacterHandle();
	}
	else
	{
		for (auto& user : everyUsers)
		{
			if (user.myID == user_id)
			{
				return user.GetCharacterHandle();
			}
		}
	}

	return nullptr;
}

bool
USagaNetworkSubSystem::GetTeam(int32 user_id, ESagaPlayerTeam& outpin)
const noexcept
{
	if (IsOfflineMode())
	{
		outpin = localUser.myTeam;

		return true;
	}
	else
	{
		for (auto& user : everyUsers)
		{
			if (user.myID == user_id)
			{
				outpin = user.myTeam;
				return true;
			}
		}
	}

	return false;
}

bool
USagaNetworkSubSystem::GetWeapon(int32 user_id, EPlayerWeapon& outpin)
const noexcept
{
	if (IsOfflineMode())
	{
		outpin = localUser.myWeapon;

		return true;
	}
	else
	{
		for (auto& user : everyUsers)
		{
			if (user.myID == user_id)
			{
				outpin = user.myWeapon;
				return true;
			}
		}
	}

	return false;
}

float
USagaNetworkSubSystem::GetHealth(int32 user_id)
const noexcept
{
	if (IsOfflineMode())
	{
		return localUser.myHealth;
	}
	else
	{
		for (auto& user : everyUsers)
		{
			if (user.myID == user_id)
			{
				return user.myHealth;
			}
		}
	}

	return -0.0f;
}

FVector
USagaNetworkSubSystem::GetStoredPosition(int32 user_id)
const noexcept
{
	if (user_id == GetLocalUserId())
	{
		return FVector{ localUser.myX, localUser.myY, localUser.myZ };
	}
	else
	{
		for (auto& user : everyUsers)
		{
			if (user.myID == user_id)
			{
				return FVector{ user.myX, user.myY, user.myZ };
			}
		}
	}

	return FVector{};
}

void
USagaNetworkSubSystem::AddRoom(const FSagaVirtualRoom& room)
{
	everyRooms.Add(room);
	wasRoomsUpdated = true;
}

void
USagaNetworkSubSystem::AddRoom(FSagaVirtualRoom&& room)
{
	everyRooms.Add(MoveTemp(room));
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
	localUser.myID = id;
}

int32
USagaNetworkSubSystem::GetLocalUserId()
const noexcept
{
	return localUser.myID;
}

void
USagaNetworkSubSystem::SetLocalUserName(FText nickname)
{
	localUser.myName = nickname;
}

FText
USagaNetworkSubSystem::GetLocalUserName()
const
{
	return localUser.myName;
}

void
USagaNetworkSubSystem::SetLocalUserTeam(ESagaPlayerTeam team)
{
	localUser.myTeam = team;
}

ESagaPlayerTeam
USagaNetworkSubSystem::GetLocalUserTeam()
const noexcept
{
	return localUser.myTeam;
}

void
USagaNetworkSubSystem::SetLocalUserWeapon(EPlayerWeapon weapon)
noexcept
{
	localUser.myWeapon = weapon;
}

EPlayerWeapon
USagaNetworkSubSystem::GetLocalUserWeapon()
const noexcept
{
	return localUser.myWeapon;
}

void
USagaNetworkSubSystem::SetCurrentRoomId(int32 id)
noexcept
{
	currentRoom.myID = id;
}

int32
USagaNetworkSubSystem::GetCurrentRoomId()
const noexcept
{
	return currentRoom.myID;
}

void
USagaNetworkSubSystem::SetCurrentRoomTitle(FText title)
{
	currentRoom.myName = title;
}

FText
USagaNetworkSubSystem::GetCurrentRoomTitle()
const
{
	return currentRoom.myName;
}

int32
USagaNetworkSubSystem::GetCurrentRoomMemberCount()
const noexcept
{
	return currentRoom.MembersCount;
}
