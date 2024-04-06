#include "Saga/Network/SagaClientPacketPrefabs.h"
#include "Saga/Serializer.h"

#include <algorithm>

std::unique_ptr<std::byte[]>
saga::cs::CS_RpcPacket::Serialize()
const
{
	return saga::Serializes(myProtocol, mySize, std::wstring_view{ rpcScript, msgLength }, rpcArgument);
}

std::byte*
saga::cs::CS_RpcPacket::Write(std::byte* buffer)
const
{
	return saga::Serialize(saga::Serialize(Super::Write(buffer), std::wstring_view{ rpcScript, msgLength }), rpcArgument);
}

const std::byte*
saga::cs::CS_RpcPacket::Read(const std::byte* buffer)
{
	return saga::Deserialize(saga::Deserialize(Super::Read(buffer), msgLength, rpcScript), rpcArgument);
}

std::unique_ptr<std::byte[]>
saga::cs::CS_CreateRoomPacket::Serialize()
const
{
	return saga::Serializes(myProtocol, mySize, std::wstring_view{ roomTitle, roomTitleLength });
}

std::byte*
saga::cs::CS_CreateRoomPacket::Write(std::byte* buffer)
const
{
	return saga::Serialize(Super::Write(buffer), std::wstring_view{ roomTitle, roomTitleLength });
}

const std::byte*
saga::cs::CS_CreateRoomPacket::Read(const std::byte* buffer)
{
	return saga::Deserialize(Super::Read(buffer), roomTitleLength, roomTitle);
}

std::unique_ptr<std::byte[]>
saga::cs::CS_UpdatePositionPacket::Serialize() const
{
	return saga::Serializes(myProtocol, mySize, x, y, z);
}

std::byte*
saga::cs::CS_UpdatePositionPacket::Write(std::byte* buffer) const
{
	return saga::Serializes(Super::Write(buffer), x, y, z);
}

const std::byte*
saga::cs::CS_UpdatePositionPacket::Read(const std::byte* buffer)
{
	return saga::Deserialize(saga::Deserialize(saga::Deserialize(Super::Read(buffer), x), y), z);
}

[[nodiscard]]
std::unique_ptr<std::byte[]>
saga::cs::CS_SignInPacket::Serialize()
const
{
	return saga::Serializes(myProtocol, mySize, std::wstring_view{ userName, nickNameLength });
}

std::byte*
saga::cs::CS_SignInPacket::Write(std::byte* buffer)
const
{
	return saga::Serialize(Super::Write(buffer), std::wstring_view{ userName, nickNameLength });
}

const std::byte*
saga::cs::CS_SignInPacket::Read(const std::byte* buffer)
{
	return saga::Deserialize(Super::Read(buffer), nickNameLength, userName);
}

IMPL_SERIALIZE_METHOD(saga::cs::CS_EnterRoomPacket, roomId);
IMPL_READ_METHODS(saga::cs::CS_EnterRoomPacket, roomId);
IMPL_WRITE_METHODS_V1(saga::cs::CS_EnterRoomPacket, roomId);

IMPL_SERIALIZE_METHOD(saga::cs::CS_SetTeamPacket, teamId);
IMPL_READ_METHODS(saga::cs::CS_SetTeamPacket, teamId);
IMPL_WRITE_METHODS_V1(saga::cs::CS_SetTeamPacket, teamId);
