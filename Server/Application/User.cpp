module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.App.User;
import Iconer.Net.Socket;
import Iconer.App.Room;
import Iconer.App.TaskContext;
import Iconer.App.SendContext;
import Iconer.App.PacketSerializer;

iconer::net::IoResult
iconer::app::User::BeginClose()
{
	mainContext->SetOperation(TaskCategory::OpClose);

	recvContext->SetOperation(TaskCategory::OpClose);

	roomContext->SetOperation(TaskCategory::OpClose);

	onDisconnected.Broadcast(this);
	SetConnected(false); // release connection

	return myReceiver.BeginClose(mainContext);
}

void
iconer::app::User::EndClose()
{
	mainContext->SetOperation(TaskCategory::None);
	mainContext->ClearIoStatus();

	recvContext->SetOperation(TaskCategory::None);

	roomContext->SetOperation(TaskCategory::None);

	myReceiver.EndClose();
}

void
iconer::app::User::Cleanup()
{
	mainContext->SetOperation(TaskCategory::None);
	mainContext->ClearIoStatus();

	recvContext->SetOperation(TaskCategory::None);
	recvContext->ClearIoStatus();

	roomContext->SetOperation(TaskCategory::None);
	roomContext->ClearIoStatus();
}

iconer::net::IoResult
iconer::app::User::SendGeneralData(iconer::app::SendContext& ctx, std::size_t length)
{
	return GetSocket().Send(ctx, ctx.myBuffer.get(), length);
}

iconer::net::IoResult
iconer::app::User::SendGeneralData(iconer::app::SendContext& ctx, std::span<const std::byte> data)
{
	return GetSocket().Send(ctx, data);
}

iconer::net::IoResult
iconer::app::User::SendGeneralData(iconer::app::SendContext& ctx, const std::byte* data, std::size_t length)
{
	return GetSocket().Send(ctx, data, length);
}

iconer::net::IoResult
iconer::app::User::SendGeneralData(iconer::app::SendContext& ctx, std::size_t length)
const
{
	return GetSocket().Send(ctx, ctx.myBuffer.get(), length);
}

iconer::net::IoResult
iconer::app::User::SendGeneralData(iconer::app::SendContext& ctx, std::span<const std::byte> data) const
{
	return GetSocket().Send(ctx, data);
}

iconer::net::IoResult
iconer::app::User::SendGeneralData(iconer::app::SendContext& ctx, const std::byte* data, std::size_t length) const
{
	return GetSocket().Send(ctx, data, length);
}

iconer::net::IoResult
iconer::app::User::SendGeneralData(iconer::app::TaskContext& ctx, std::span<const std::byte> data)
{
	return GetSocket().Send(ctx, data);
}

iconer::net::IoResult
iconer::app::User::SendGeneralData(iconer::app::TaskContext& ctx, const std::byte* data, std::size_t length)
{
	return GetSocket().Send(ctx, data, length);
}

iconer::net::IoResult
iconer::app::User::SendGeneralData(iconer::app::TaskContext& ctx, std::span<const std::byte> data)
const
{
	return GetSocket().Send(ctx, data);
}

iconer::net::IoResult
iconer::app::User::SendGeneralData(iconer::app::TaskContext& ctx, const std::byte* data, std::size_t length)
const
{
	return GetSocket().Send(ctx, data, length);
}

iconer::net::IoResult
iconer::app::User::SendSignInPacket()
{
	return GetSocket().Send(mainContext, mainContext->GetSignInPacketData());
}

iconer::net::IoResult
iconer::app::User::SendFailedSignInPacket(iconer::app::ConnectionContract reason)
{
	return GetSocket().Send(mainContext, mainContext->GetSignInFailurePacketData());
}

iconer::net::IoResult
iconer::app::User::SendRoomCreatedPacket(id_type room_id)
{
	return GetSocket().Send(roomContext, mainContext->GetRoomCreatedPacketData(static_cast<std::int32_t>(room_id)));
}

iconer::net::IoResult
iconer::app::User::SendFailedCreateRoomPacket(iconer::app::TaskContext& ctx, iconer::app::RoomContract reason)
{
	return GetSocket().Send(ctx, mainContext->GetRoomCreateFailedPacketData(reason));
}

iconer::net::IoResult
iconer::app::User::SendJoinedRoomPacket(id_type room_id)
{
	return GetSocket().Send(roomContext, mainContext->GetRoomJoinedPacketData(static_cast<std::int32_t>(room_id)));
}

iconer::net::IoResult
iconer::app::User::SendRoomJoinFailedPacket(iconer::app::RoomContract reason)
{
	return GetSocket().Send(roomContext, mainContext->GetRoomJoinFailedPacketData(reason));
}

iconer::net::IoResult
iconer::app::User::BeginOptainReceiveMemory()
{
	recvContext->SetOperation(TaskCategory::OpOptainRecvMemory);

	return myReceiver.BeginOptainMemory(recvContext);
}

bool
iconer::app::User::EndOptainReceiveMemory(bool flag)
noexcept
{
	myReceiver.EndOptainMemory(recvContext);

	SetConnected(flag);

	if (flag) LIKELY
	{
		return recvContext->TryChangeOperation(TaskCategory::OpOptainRecvMemory, TaskCategory::OpRecv);
	}
	else UNLIKELY
	{
		return recvContext->TryChangeOperation(TaskCategory::OpOptainRecvMemory, TaskCategory::None);
	}
}
