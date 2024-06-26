module;

module Iconer.App.User;

std::expected<void, iconer::net::ErrorCode>
iconer::app::User::BeginClose()
{
	mainContext->SetOperation(TaskCategory::OpClose);

	myReceiver.mySocket.AsyncClose(mainContext);

	return std::expected<void, iconer::net::ErrorCode>();
}

void
iconer::app::User::EndClose()
{}

void
iconer::app::User::Cleanup()
{

}
