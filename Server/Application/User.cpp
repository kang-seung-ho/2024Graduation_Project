module;

module Iconer.App.User;

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

	recvContext->ClearIoStatus();

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

	if (flag)
	{
		return recvContext->TryChangeOperation(TaskCategory::OpOptainRecvMemory, TaskCategory::OpRecv);
	}
	else
	{
		return recvContext->TryChangeOperation(TaskCategory::OpOptainRecvMemory, TaskCategory::None);
	}
}
