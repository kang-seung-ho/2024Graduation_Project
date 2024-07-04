module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.App.User;
import Iconer.App.SendContext;

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
iconer::app::User::SendFailedCreateRoomPacket(iconer::app::SendContext& ctx, iconer::app::RoomContract reason)
{
	return GetSocket().Send(ctx, mainContext->GetRoomCreateFailedPacketData(reason));
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
