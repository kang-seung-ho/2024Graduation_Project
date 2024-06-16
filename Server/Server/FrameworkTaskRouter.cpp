#include "Framework.hpp"

import Iconer.Net;
import Iconer.App.TaskContext;

import <utility>;
import <print>;

void
ServerFramework::OnTaskSucceed(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes) const
{
	const auto task_ctx = static_cast<iconer::app::TaskContext*>(context);

	if (nullptr == task_ctx)
	{
		return;
	}

	using namespace iconer::net;
	using enum iconer::app::TaskCategory;

	switch (task_ctx->myCategory)
	{
	case OpReserve:
	{
		const auto user = userManager.FindSession(id);
		if (nullptr == user)
		{
			throw "ReserveError!";
		}

		// task_ctx is the mainContext of user
		task_ctx->SetOperation(OpAccept);
		task_ctx->ClearIoStatus();

		if (auto io = listenSocket.BeginAccept(task_ctx, user->GetSocket()); io)
		{
			std::println("User {} is reserved.", id);
		}
		else
		{
			std::println("Reservation of user {} is failed, due to {}.", id, std::to_string(io.error()));

			throw "ReserveError!";
		}
	}
	break;

	case OpAccept:
	{
		const auto user = userManager.FindSession(id);
		if (nullptr == user)
		{
			throw "AcceptError!";
		}

		// task_ctx is the mainContext of user
		task_ctx->SetOperation(OpValidation);
		task_ctx->ClearIoStatus();

		auto& socket = user->GetSocket();
		if (auto io = socket.EndAccept(listenSocket); io)
		{
			std::println("User {} is just accepted.", id);
		}
		else
		{
			std::println("Acceptance of user {} is failed, due to {}.", id, std::to_string(io.error()));

			throw "AcceptError!";
		}

		if (auto io = user->BeginReceive(); io)
		{
			std::println("User {} started receiving.", id);

			user->SetConnected(true);
		}
		else
		{
			std::println("User {} has failed to start receiving, due to {}.", id, std::to_string(io.error()));

			auto& recv_ctx = user->recvContext;
			recv_ctx.ClearIoStatus();

			// restart
			task_ctx->SetOperation(OpReserve);
			myTaskPool.Schedule(task_ctx, id);
		}
	}
	break;

	case OpRecv:
	{
		const auto user = userManager.FindSession(id);
		if (nullptr == user)
		{
			std::println("Unknown Receive from id {} ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		user->EndReceive(bytes);
	}
	break;

	default:
	{
		std::println("[Task({})] id={} ({} bytes)", static_cast<void*>(context), id, bytes);
		task_ctx->ClearIoStatus();
	}
	break;
	}
}

void
ServerFramework::OnTaskFailure(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes) const
{
	const auto task_ctx = static_cast<iconer::app::TaskContext*>(context);

	if (nullptr == task_ctx)
	{
		std::println("[Null Task]id={} ({} bytes)", id, bytes);
		return;
	}

	using namespace iconer::net;
	using enum iconer::app::TaskCategory;

	switch (task_ctx->myCategory)
	{
	case OpRecv:
	{
		const auto user = userManager.FindSession(id);
		if (nullptr == user)
		{
			std::println("Unknown Receive from id {} ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		user->SetConnected(false);
	}
	break;

	default:
	{
		std::println("[Task({})] id={} ({} bytes)", static_cast<void*>(context), id, bytes);
		task_ctx->ClearIoStatus();
	}
	break;
	}
}

void
ServerFramework::ReserveUser(iconer::app::User& user)
const noexcept
{
	auto& ctx = user.mainContext;

	ctx.SetOperation(iconer::app::TaskCategory::OpReserve);
	myTaskPool.Schedule(ctx, user.GetID());
}
