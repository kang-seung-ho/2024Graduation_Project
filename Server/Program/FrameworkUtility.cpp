module Iconer.Framework;

import Iconer.Net.Socket;
import Iconer.App.User;

namespace
{
	iconer::util::AtomicQueue<iconer::app::SendContext*, ServerFramework::reservedSendContextCount> storedSendContexts{};
}

iconer::net::IoResult
ServerFramework::AcceptUser(iconer::app::User& user)
{
	auto& ctx = user.mainContext;
	ctx->SetOperation(iconer::app::TaskCategory::OpValidation);

	return user.GetSocket().EndAccept(GetListenSocket());
}

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
ServerFramework::AddSendContext(iconer::app::SendContext* context)
noexcept
{
	storedSendContexts.push(context);
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

void
ServerFramework::AddRpcProcessor(iconer::app::RpcProtocol protocol, const RpcDelegate& processor)
{
	rpcProcessors[static_cast<size_t>(protocol)] = processor;
}
