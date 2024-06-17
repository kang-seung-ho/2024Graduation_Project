#include "Framework.hpp"

import Iconer.App.User;

void
ServerFramework::ReserveUser(iconer::app::User& user)
const noexcept
{
	auto& ctx = user.mainContext;

	ctx->SetOperation(iconer::app::TaskCategory::OpReserve);
	myTaskPool.Schedule(ctx, user.GetID());
}

iconer::net::Socket::IoResult
ServerFramework::TriggerUser(iconer::app::User& user)
const noexcept
{
	auto io = user.BeginReceive();

	if (io)
	{
		user.SetConnected(true);
	}
	else
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
