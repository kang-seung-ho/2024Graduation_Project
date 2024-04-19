module TestClient;
import Iconer.Application.BasicPacket;
import Iconer.Application.Packet;

test::IoResult
test::SendSignInPacket()
{
	const iconer::app::packets::CS_SignInPacket pk{ my_name.data(), my_name.size() };

	auto ptr = pk.Serialize();

	auto r = app_socket.Send(ptr.get(), pk.WannabeSize());
	delete[] ptr.release();

	return std::move(r);
}

test::IoResult
test::SendCreateRoomPacket(std::wstring_view title)
{
	iconer::app::packets::CS_CreateRoomPacket pk{ title.data(), title.length() };
	std::copy_n(title.cbegin(), std::min(title.length(), pk.roomTitleLength), pk.roomTitle);

	auto ptr = pk.Serialize();

	auto r = app_socket.Send(ptr.get(), pk.WannabeSize());
	delete[] ptr.release();

	return std::move(r);
}

test::IoResult
test::SendJoinRoomPacket(IdType room_id)
{
	iconer::app::packets::CS_EnterRoomPacket pk{};
	pk.roomId = room_id;

	auto ptr = pk.Serialize();

	return app_socket.Send(ptr.get(), pk.WannabeSize());
}

test::IoResult
test::SendLeaveRoomPacket()
{
	static constexpr iconer::app::packets::CS_LeaveRoomPacket pk{};
	static auto ptr = pk.Serialize();

	return app_socket.Send(ptr.get(), pk.WannabeSize());
}

test::IoResult
test::SendRequestVersionPacket()
{
	static constexpr iconer::app::packets::CS_RequestVersionPacket pk{};
	static auto ptr = pk.Serialize();

	return app_socket.Send(ptr.get(), pk.WannabeSize());
}

test::IoResult
test::SendRequestRoomsPacket()
{
	static constexpr iconer::app::packets::CS_RequestRoomsPacket pk{};
	static auto ptr = pk.Serialize();

	return app_socket.Send(ptr.get(), pk.WannabeSize());
}

test::IoResult
test::SendRequestMembersPacket()
{
	static constexpr iconer::app::packets::CS_RequestMembersPacket pk{};
	static auto ptr = pk.Serialize();

	return app_socket.Send(ptr.get(), pk.WannabeSize());
}

test::IoResult
test::SendGameStartPacket()
{
	static constexpr iconer::app::packets::CS_GameStartPacket pk{};
	static auto ptr = pk.Serialize();

	return app_socket.Send(ptr.get(), pk.WannabeSize());
}

test::IoResult
test::SendGameIsLoadedPacket()
{
	static constexpr iconer::app::packets::CS_GameLoadedPacket pk{};
	static auto ptr = pk.Serialize();

	return app_socket.Send(ptr.get(), pk.WannabeSize());
}

test::IoResult
test::SendRpcPacket(iconer::app::RpcProtocol cat, std::int64_t arg0, std::int32_t arg1)
{
	const iconer::app::packets::CS_DeterRpcPacket pk{ cat, arg0, arg1 };
	auto ptr = pk.Serialize();

	return app_socket.Send(ptr.get(), pk.WannabeSize());
}

bool
test::SendPositionPacket()
{
	iconer::app::packets::CS_UpdatePositionPacket position_pk{};
	position_pk.x = localPlayer->x;
	position_pk.y = localPlayer->y;
	position_pk.z = localPlayer->z;

	auto serialized = position_pk.Serialize();

	auto sent_position_r = app_socket.Send(serialized.get(), iconer::app::packets::CS_UpdatePositionPacket::WannabeSize());
	if (not sent_position_r.has_value())
	{
		return false;
	}

	return true;
}
