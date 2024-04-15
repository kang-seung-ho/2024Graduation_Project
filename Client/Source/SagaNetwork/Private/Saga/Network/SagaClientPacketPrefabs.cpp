#include "Saga/Network/SagaClientPacketPrefabs.h"
#include "Saga/Serializer.h"

#include <algorithm>

std::unique_ptr<std::byte[]>
saga::cs::CS_RpcPacket::Serialize()
const
{
	return saga::Serializes(myProtocol, mySize, std::string_view{ rpcScript, msgLength }, rpcArgument);
}

std::byte*
saga::cs::CS_RpcPacket::Write(std::byte* buffer)
const
{
	return saga::Serialize(saga::Serialize(Super::Write(buffer), std::string_view{ rpcScript, msgLength }), rpcArgument);
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

IMPL_SERIALIZE_METHOD(saga::cs::CS_DeterRpcPacket, rpcProtocol, rpcArgument0, rpcArgument1);
IMPL_WRITE_METHOD(saga::cs::CS_DeterRpcPacket, rpcProtocol, rpcArgument0, rpcArgument1);
IMPL_READ_METHOD_V3(saga::cs::CS_DeterRpcPacket, rpcProtocol, rpcArgument0, rpcArgument1);

IMPL_SERIALIZE_METHOD(saga::cs::CS_UpdatePositionPacket, x, y, z);
IMPL_WRITE_METHOD(saga::cs::CS_UpdatePositionPacket, x, y, z);
IMPL_READ_METHOD_V3(saga::cs::CS_UpdatePositionPacket, x, y, z);

IMPL_SERIALIZE_METHOD(saga::cs::CS_UpdateRotationPacket, r, y, p);
IMPL_WRITE_METHOD(saga::cs::CS_UpdateRotationPacket, r, y, p);
IMPL_READ_METHOD_V3(saga::cs::CS_UpdateRotationPacket, r, y, p);

IMPL_SERIALIZE_METHOD(saga::cs::CS_EnterRoomPacket, roomId);
IMPL_WRITE_METHOD(saga::cs::CS_EnterRoomPacket, roomId);
IMPL_READ_METHOD_V1(saga::cs::CS_EnterRoomPacket, roomId);

IMPL_SERIALIZE_METHOD(saga::cs::CS_SetTeamPacket, teamId);
IMPL_WRITE_METHOD(saga::cs::CS_SetTeamPacket, teamId);
IMPL_READ_METHOD_V1(saga::cs::CS_SetTeamPacket, teamId);
