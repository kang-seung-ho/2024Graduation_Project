#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Saga/Network/SagaPacketSenders.h"

int32
USagaNetworkSubSystem::SendSignInPacket(const FString& nickname)
{
	return saga::SendSignInPacket(clientSocket, nickname).value_or(0);
}

int32
USagaNetworkSubSystem::SendCreateRoomPacket(const FString& title)
{
	return saga::SendCreateRoomPacket(clientSocket, title).value_or(0);
}

int32
USagaNetworkSubSystem::SendJoinRoomPacket(int32 room_id)
{
	return saga::SendJoinRoomPacket(clientSocket, room_id).value_or(0);
}

int32
USagaNetworkSubSystem::SendLeaveRoomPacket()
{
	return saga::SendLeaveRoomPacket(clientSocket).value_or(0);
}

int32
USagaNetworkSubSystem::SendRequestVersionPacket()
{
	return saga::SendRequestVersionPacket(clientSocket).value_or(0);
}

int32
USagaNetworkSubSystem::SendRequestRoomsPacket()
{
	return saga::SendRequestRoomsPacket(clientSocket).value_or(0);
}

int32
USagaNetworkSubSystem::SendRequestMembersPacket()
{
	return saga::SendRequestMembersPacket(clientSocket).value_or(0);
}

int32
USagaNetworkSubSystem::SendChangeTeamPacket(bool is_red_team)
{
	return saga::SendChangeTeamPacket(clientSocket, is_red_team).value_or(0);
}

int32
USagaNetworkSubSystem::SendGameStartPacket()
{
	return saga::SendGameStartPacket(clientSocket).value_or(0);
}

int32
USagaNetworkSubSystem::SendGameIsLoadedPacket()
{
	return saga::SendGameIsLoadedPacket(clientSocket).value_or(0);
}

int32
USagaNetworkSubSystem::SendPositionPacket(float x, float y, float z)
{
	return saga::SendPositionPacket(clientSocket, x, y, z).value_or(0);
}

int32
USagaNetworkSubSystem::SendRotationPacket(float r, float y, float p)
{
	return saga::SendRotationPacket(clientSocket, r, y, p).value_or(0);
}

int32
USagaNetworkSubSystem::SendRpcPacket(const FString& string, int64 argument)
{
	return saga::SendRpcPacket(clientSocket, string, MoveTemp(argument)).value_or(0);
}
