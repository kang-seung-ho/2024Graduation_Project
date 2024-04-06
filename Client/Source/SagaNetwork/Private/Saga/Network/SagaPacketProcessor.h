#pragma once
#ifndef SAGAFRAMEWORK_NET_PACKET_PROCESSOR_H
#define SAGAFRAMEWORK_NET_PACKET_PROCESSOR_H

#include <cstddef>
#include "SagaNetwork.h"

#include "Saga/Network/SagaRoomContract.h"
#include "Saga/Network/SagaServerPacketPrefabs.h"

namespace saga
{
	const std::byte* ReceiveSignInSucceedPacket(const std::byte* buffer, int32& my_id);
	const std::byte* ReceiveSignInFailurePacket(const std::byte* buffer, int32& error);

	const std::byte* ReceiveRoomCreatedPacket(const std::byte* buffer, int32& room_id);
	const std::byte* ReceiveRoomCreationFailedPacket(const std::byte* buffer, ERoomContract& error);
	const std::byte* ReceiveRoomJoinedPacket(const std::byte* buffer, datagrams::SerializedMember& client, int32& room_id, const int32& my_id);
	const std::byte* ReceiveRoomJoinFailedPacket(const std::byte* buffer, ERoomContract& error);
	const std::byte* ReceiveRoomLeftPacket(const std::byte* buffer, int32& client_id);

	const std::byte* ReceiveRespondVersionPacket(const std::byte* buffer, wchar_t* version_str_buffer, const size_t& str_length);
	const std::byte* ReceiveRespondRoomsPacket(const std::byte* buffer, std::vector<datagrams::SerializedRoom>& rooms);
	const std::byte* ReceiveRespondUsersPacket(const std::byte* buffer, std::vector<datagrams::SerializedMember>& users);

	const std::byte* ReceiveTeamChangerPacket(const std::byte* buffer, int32& client_id, bool& is_red_team);

	const std::byte* ReceivePositionPacket(const std::byte* buffer, int32& client_id, float& x, float& y, float& z);

	const std::byte* ReceiveRpcPacket(const std::byte* buffer, int32& client_id, FString& contents, long long& argument);
	const std::byte* ReceiveRpcPacket(const std::byte* buffer, int32& client_id, FString& contents);
}

#endif // !SAGAFRAMEWORK_NET_PACKET_PROCESSOR_H
