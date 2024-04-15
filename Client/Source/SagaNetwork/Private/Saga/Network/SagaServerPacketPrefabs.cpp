#include "Saga/Network/SagaServerPacketPrefabs.h"
#include "Saga/Serializer.h"

#include <algorithm>

const std::byte*
saga::sc::SC_RpcPacket::Read(const std::byte* buffer)
{
	return saga::Deserialize(saga::Deserialize(saga::Deserialize(Super::Read(buffer), clientId), msgLength, rpcScript), rpcArgument);
}

const std::byte*
saga::sc::SC_RespondVersionPacket::Read(const std::byte* buffer)
{
	return saga::Deserialize(Super::Read(buffer), versionLength, gameVersion);
}

const std::byte*
saga::sc::SC_RespondRoomsPacket::Read(const std::byte* buffer)
{
	auto seek = Super::Read(buffer);

	size_t size{};
	seek = saga::Deserialize(seek, size);

	if (0 < size)
	{
		serializedRooms.clear();
		serializedRooms.resize(size);

		for (auto& room : serializedRooms)
		{
			seek = saga::Deserialize(seek, room.id);
			seek = saga::Deserialize(seek, room.nameLength, room.title);
			seek = saga::Deserialize(seek, room.members);
		}
	}

	return seek;
}

const std::byte*
saga::sc::SC_RespondMembersPacket::Read(const std::byte* buffer)
{
	auto seek = Super::Read(buffer);

	size_t size{};
	seek = saga::Deserialize(seek, size);

	if (0 < size)
	{
		serializedMembers.clear();
		serializedMembers.resize(size);

		for (auto& member : serializedMembers)
		{
			seek = saga::Deserialize(seek, member.id);
			seek = saga::Deserialize(seek, member.team_id);
			seek = saga::Deserialize(seek, member.nameLength, member.nickname);
		}
	}

	return seek;
}

const std::byte*
saga::sc::SC_CreatePlayerPacket::Read(const std::byte* buffer)
{
	return saga::Deserialize(saga::Deserialize(Super::Read(buffer), clientId), nickNameLength, userName);
}

const std::byte*
saga::sc::SC_RoomOtherJoinedPacket::Read(const std::byte* buffer)
{
	auto seek = Super::Read(buffer);

	seek = saga::Deserialize(seek, memberInfo.id);
	seek = saga::Deserialize(seek, memberInfo.team_id);
	seek = saga::Deserialize(seek, memberInfo.nameLength, memberInfo.nickname);
	seek = saga::Deserialize(seek, roomId);

	return seek;
}

IMPL_READ_METHOD_V1(saga::sc::SC_SucceedSignInPacket, clientId);

IMPL_READ_METHOD_V1(saga::sc::SC_FailedSignInPacket, errCause);

IMPL_READ_METHOD_V1(saga::sc::SC_FailedGameStartingPacket, errCause);

IMPL_READ_METHOD_V1(saga::sc::SC_RoomCreatedPacket, roomId);

IMPL_READ_METHOD_V1(saga::sc::SC_RoomCreationFailedPacket, errCause);

IMPL_READ_METHOD_V2(saga::sc::SC_RoomJoinedPacket, clientId, roomId);

IMPL_READ_METHOD_V1(saga::sc::SC_RoomJoinFailedPacket, errCause);

IMPL_READ_METHOD_V1(saga::sc::SC_RoomLeftPacket, clientId);

IMPL_READ_METHOD_V2(saga::sc::SC_SetTeamPacket, clientId, teamId);

IMPL_READ_METHOD_V4(saga::sc::SC_UpdatePositionPacket, clientId, x, y, z);
IMPL_READ_METHOD_V4(saga::sc::SC_UpdateRotationPacket, clientId, r, y, p);

IMPL_READ_METHOD_V1(saga::sc::SC_DestroyPlayerPacket, clientId);
