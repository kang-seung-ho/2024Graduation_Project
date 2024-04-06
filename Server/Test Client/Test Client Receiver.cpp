module TestClient;
import Iconer.Application.BasicPacket;
import Iconer.Application.Packet;

const std::byte*
test::ReceiveSignInSucceedPacket(const std::byte* buffer)
{
	iconer::app::packets::SC_SucceedSignInPacket pk{};
	const std::byte* offset = pk.Read(recv_space);

	my_id = pk.clientId;

	auto r = everyClients.emplace(std::make_pair(pk.clientId, FSagaPlayer{}));

	localPlayer = std::addressof(r.first->second);
	localPlayer->id = pk.clientId;
	localPlayer->nickname = my_name;

	return offset;
}

const std::byte*
test::ReceiveSignInFailurePacket(const std::byte* buffer, int& error)
{
	iconer::app::packets::SC_FailedSignInPacket pk{};
	auto offset = pk.Read(recv_space);

	error = pk.errCause;

	return offset;
}

const std::byte*
test::ReceiveRoomCreatedPacket(const std::byte* buffer, IdType& room_id)
{
	iconer::app::packets::SC_RoomCreatedPacket pk{};
	auto offset = pk.Read(recv_space);

	room_id = pk.roomId;

	return offset;
}

const std::byte*
test::ReceiveRoomCreationFailedPacket(const std::byte* buffer, iconer::app::RoomContract& error)
{
	iconer::app::packets::SC_RoomCreationFailedPacket pk{};
	auto offset = pk.Read(recv_space);

	error = pk.errCause;

	return offset;
}

const std::byte*
test::ReceiveRoomJoinedPacket(const std::byte* buffer, IdType& client_id, IdType& room_id)
{
	iconer::app::packets::SC_RoomOtherJoinedPacket pk{};
	auto offset = pk.Read(recv_space);

	client_id = pk.memberInfo.id;
	room_id = pk.roomId;

	return offset;
}

const std::byte*
test::ReceiveRoomJoinFailedPacket(const std::byte* buffer, iconer::app::RoomContract& error)
{
	iconer::app::packets::SC_RoomJoinFailedPacket pk{};
	auto offset = pk.Read(recv_space);

	error = pk.errCause;

	return offset;
}

const std::byte*
test::ReceiveRoomLeftPacket(const std::byte* buffer, IdType& client_id)
{
	iconer::app::packets::SC_RoomLeftPacket pk{};
	auto offset = pk.Read(recv_space);

	client_id = pk.clientId;

	return offset;
}
