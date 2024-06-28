module;

module Iconer.App.User;

std::expected<void, iconer::net::ErrorCode>
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
