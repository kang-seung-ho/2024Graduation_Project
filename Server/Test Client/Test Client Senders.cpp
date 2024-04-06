module TestClient;
import Iconer.Application.BasicPacket;
import Iconer.Application.Packet;

iconer::net::Socket::IoResult
test::SendSignInPacket()
{
	const iconer::app::packets::CS_SignInPacket sign_packet{ my_name.data(), my_name.size() };

	auto ptr = sign_packet.Serialize();

	return app_socket.Send(ptr.get(), sign_packet.WannabeSize());
}

iconer::net::Socket::IoResult
test::SendCreateRoomPacket(std::wstring_view title)
{
	const iconer::app::packets::CS_CreateRoomPacket pk{ title.data(), title.length() };

	auto ptr = pk.Serialize();

	auto r = app_socket.Send(ptr.get(), pk.WannabeSize());
	delete[] ptr.release();

	return std::move(r);
}

iconer::net::Socket::IoResult
test::SendJoinRoomPacket(IdType room_id)
{
	const iconer::app::packets::CS_EnterRoomPacket pk{ room_id };

	auto ptr = pk.Serialize();

	return app_socket.Send(ptr.get(), pk.WannabeSize());
}

iconer::net::Socket::IoResult
test::SendLeaveRoomPacket()
{
	static constexpr iconer::app::packets::CS_LeaveRoomPacket pk{};
	static auto ptr = pk.Serialize();

	return app_socket.Send(ptr.get(), pk.WannabeSize());
}

iconer::net::Socket::IoResult
test::SendRequestVersionPacket()
{
	static constexpr iconer::app::packets::CS_RequestVersionPacket pk{};
	static auto ptr = pk.Serialize();

	return app_socket.Send(ptr.get(), pk.WannabeSize());
}

iconer::net::Socket::IoResult
test::SendRequestRoomsPacket()
{
	static constexpr iconer::app::packets::CS_RequestRoomsPacket pk{};
	static auto ptr = pk.Serialize();

	return app_socket.Send(ptr.get(), pk.WannabeSize());
}

iconer::net::Socket::IoResult
test::SendRequestMembersPacket()
{
	static constexpr iconer::app::packets::CS_RequestMembersPacket pk{};
	static auto ptr = pk.Serialize();

	return app_socket.Send(ptr.get(), pk.WannabeSize());
}

iconer::net::Socket::IoResult
test::SendGameStartPacket()
{
	static constexpr iconer::app::packets::CS_GameStartPacket pk{};
	static auto ptr = pk.Serialize();

	return iconer::net::Socket::IoResult();
}

iconer::net::Socket::IoResult
test::SendGameIsLoadedPacket()
{
	static constexpr iconer::app::packets::CS_GameLoadedPacket pk{};
	static auto ptr = pk.Serialize();

	return iconer::net::Socket::IoResult();
}

bool
test::SendPositionPacket()
{
	iconer::app::packets::CS_UpdatePositionPacket position_pk{ localPlayer->x, localPlayer->y, localPlayer->z };

	auto serialized = position_pk.Serialize();

	auto sent_position_r = app_socket.Send(serialized.get(), iconer::app::packets::CS_UpdatePositionPacket::WannabeSize());
	if (not sent_position_r.has_value())
	{
		return false;
	}

	return true;
}
