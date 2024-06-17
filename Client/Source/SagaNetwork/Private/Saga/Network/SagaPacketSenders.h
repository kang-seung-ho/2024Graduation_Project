#pragma once
#ifndef SAGAFRAMEWORK_NET_PACKET_SENDERS_H
#define SAGAFRAMEWORK_NET_PACKET_SENDERS_H

#include "SagaNetwork.h"
#include "Saga/Network/SagaRpcProtocol.h"
#include <optional>

namespace saga
{
	std::optional<int32> SendSignInPacket(FSocket* socket, FStringView nickname);
	std::optional<int32> SendCreateRoomPacket(FSocket* socket, FStringView title);
	std::optional<int32> SendJoinRoomPacket(FSocket* socket, int32 room_id);
	std::optional<int32> SendLeaveRoomPacket(FSocket* socket);
	std::optional<int32> SendRequestVersionPacket(FSocket* socket);
	std::optional<int32> SendRequestRoomsPacket(FSocket* socket);
	std::optional<int32> SendRequestMembersPacket(FSocket* socket);
	std::optional<int32> SendChangeTeamPacket(FSocket* socket, bool is_red_team);
	std::optional<int32> SendGameStartPacket(FSocket* socket);
	std::optional<int32> SendGameIsLoadedPacket(FSocket* socket);
	std::optional<int32> SendPositionPacket(FSocket* socket, float x, float y, float z);
	std::optional<int32> SendRotationPacket(FSocket* socket, float r, float y, float p);
	std::optional<int32> SendRpcPacket(FSocket* socket, ESagaRpcProtocol cat, int32 user_id, int64 arg0 = 0, int32 arg1 = 0);
}

#endif // !SAGAFRAMEWORK_NET_PACKET_SENDERS_H
