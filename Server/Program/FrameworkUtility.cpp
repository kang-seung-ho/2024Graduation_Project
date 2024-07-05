module Iconer.Framework;

import Iconer.App.User;
import Iconer.App.SendContext;

void
ServerFramework::ReserveUser(iconer::app::User& user)
const noexcept
{
	auto& ctx = user.mainContext;

	ctx->SetOperation(iconer::app::TaskCategory::OpReserve);
	myTaskPool.Schedule(ctx, user.GetID());
}

iconer::net::IoResult
ServerFramework::TriggerUser(iconer::app::User& user)
const noexcept
{
	auto io = user.BeginOptainReceiveMemory();

	if (not io)
	{
		auto& recv_ctx = user.recvContext;
		recv_ctx->ClearIoStatus();
	}

	return std::move(io);
}

iconer::net::IoResult
ServerFramework::StartUser(iconer::app::User& user)
const
{
	auto io = user.BeginReceive();

	if (not io)
	{
		auto& recv_ctx = user.recvContext;
		recv_ctx->ClearIoStatus();
	}

	return std::move(io);
}

void
ServerFramework::CleanupUser(iconer::app::User& user)
const
{

}

iconer::app::SendContext*
ServerFramework::AcquireSendContext()
noexcept
{
	return storedSendContexts.pop();
}
void
ServerFramework::AddPacketProcessor(iconer::app::PacketProtocol protocol, const EventDelegate& processor)
{
	packetProcessors.insert(std::make_pair(protocol, processor));
}
