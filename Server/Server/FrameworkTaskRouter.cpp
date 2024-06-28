#include "Framework.hpp"

import Iconer.Net;
import Iconer.App.User;
import Iconer.App.TaskContext;
import Iconer.App.UserContext;
import Iconer.App.PacketContext;

import <utility>;
import <print>;

void
ServerFramework::OnTaskSucceed(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes)
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
	case OpSend:
	{
		delete task_ctx;
	}
	break;
	
	case OpPacketProcess:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			std::println("Unknown packet processor from id {} ({} bytes)", id, bytes);

			delete context;
			break;
		}
		else
		{
			std::println("Packet processor from id {} ({} bytes)", id, bytes);
		}

		const auto packet_ctx = static_cast<iconer::app::PacketContext*>(context);

		packet_ctx->ClearIoStatus();
		packet_ctx->myData.reset();
		storedPacketContexts.push(packet_ctx);
	}
	break;
	
	case OpClose:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			std::println("Unknown closing from id {} ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		user->EndClose();

		std::println("User {} is just closed", id);

		// restart
		ReserveUser(*user);
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

		if (0 == bytes)
		{
			std::println("User {} is just disconnected.", id);

			user->BeginClose();

			break;
		}

		std::println("User {} received {} bytes.", id, bytes);

		//auto buffer = user->AcquireReceivedData();
		RoutePackets(*user);

		if (auto io = user->BeginReceive(); io)
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

	case OpOptainRecvMemory:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			std::println("Unknown receive memory optaining from id {} ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		if (not user->EndOptainReceiveMemory(true))
		{
			std::println("User {} optained its memory for receiving, but the task has an invalid state.", id);

			// Atomic errors would not be detected by io completion port
			user->BeginClose();

			break;
		}

		if (auto io = StartUser(*user); io)
		{
			std::println("User {} started receiving.", id);
		}
		else
		{
			// Would trigger OnTaskFailure
			std::println("User {} has failed to start receiving, due to {}.", id, std::to_string(io.error()));
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
			std::println("User {} started optaining its memory for receiving.", user_id);
		}
		else
		{
			// Would trigger OnTaskFailure
			std::println("User {} failed to optain its memory for receiving, due to {}.", user_id, std::to_string(io.error()));
		}
	}
	break;

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

	default:
	{
		std::println("[Task({})] id={} ({} bytes)", static_cast<void*>(context), id, bytes);
		task_ctx->ClearIoStatus();
	}
	break;
	}
}

void
ServerFramework::OnTaskFailure(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes)
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
	case OpOptainRecvMemory:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			std::println("Unknown failed receive memory optaining from id {} ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		std::println("User {} has failed on optaining its memory for receiving. (task={})", id, user->EndOptainReceiveMemory(false));

		user->BeginClose();
	}
	break;
	
	case OpRecv:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			std::println("Unknown failed receive from id {} ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		user->BeginClose();
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
