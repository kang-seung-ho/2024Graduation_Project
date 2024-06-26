#include "Framework.hpp"

import Iconer.Net;
import Iconer.App.TaskContext;
import Iconer.App.UserContext;
import Iconer.App.PacketContext;

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
		const auto user = userManager.FindUser(id);
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
		const auto ctx = static_cast<iconer::app::UserContext*>(context);

		auto user = ctx->ownerHandle;

		if (nullptr == user)
		{
			throw "AcceptError!";
		}

		// task_ctx is the mainContext of user
		task_ctx->SetOperation(OpValidation);
		task_ctx->ClearIoStatus();

		const auto user_id = user->GetID();

		auto& socket = user->GetSocket();
		if (auto io = socket.EndAccept(listenSocket); io)
		{
			std::println("User {} is just accepted.", user_id);
		}
		else
		{
			std::println("Acceptance of user {} is failed, due to {}.", user_id, std::to_string(io.error()));

			throw "AcceptError!";
		}

		if (auto io = TriggerUser(*user); io)
		{
			std::println("User {} started receiving.", user_id);
		}
		else
		{
			// Would trigger OnTaskFailure
			std::println("User {} has failed to start receiving, due to {}.", user_id, std::to_string(io.error()));
		}
	}
	break;

	case OpRecv:
	{
		const auto user = userManager.FindUser(id);
		if (nullptr == user)
		{
			std::println("Unknown receive from id {} ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		user->EndReceive(bytes);

		//auto buffer = user->AcquireReceivedData();
		ProcessPacket(*user);

		if (auto io = TriggerUser(*user); io)
		{
			std::println("User {} restarted receiving.", id);
		}
		else
		{
			// Would trigger OnTaskFailure
			std::println("User {} has failed to restart receiving, due to {}.", id, std::to_string(io.error()));
		}
	}
	break;

	case OpSend:
	{
		delete task_ctx;
	}
	break;
	
	case OpPacketProcess:
	{
		delete task_ctx;
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
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			std::println("Unknown failed receive from id {} ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		user->onDisconnected.Broadcast(user);
		user->SetConnected(false); // release connection

		// restart
		ReserveUser(*user);
	}
	break;

	case OpSend:
	{
		delete task_ctx;
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
