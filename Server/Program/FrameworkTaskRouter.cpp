module;
#include <cstring>

#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Framework;
import iconer.Utility.Serializer;
import Iconer.Net;
import Iconer.Net.Socket;
import Iconer.App.User;
import Iconer.App.Room;
import Iconer.App.TaskContext;
import Iconer.App.UserContext;
import Iconer.App.SendContext;
import Iconer.App.PacketContext;

void
ServerFramework::OnTaskSucceed(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes)
{
	const auto task_ctx = static_cast<iconer::app::TaskContext*>(context);

	if (nullptr == task_ctx)
	{
		return;
	}

	task_ctx->ClearIoStatus();

	using namespace iconer::net;
	using enum iconer::app::TaskCategory;

	switch (task_ctx->myCategory)
	{
	case OpStartGame:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown game starter from id {}. ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		EventOnGameStarted(*user);
	}
	break;

	case OpSpreadGameTicket:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown failed game ticket from id {}. ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		EventOnSpreadGameTickets(*user);
	}
	break;

	case OpCreateGame:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown failed creating a game from id {}. ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		EventOnMakeGame(*user);
	}
	break;

	case OpNotifyTeam:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown failed notifying team from id {}. ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		EventOnNotifyTeamChanged(*user, bytes);
	}
	break;

	case OpEnterRoom:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user) UNLIKELY
		{
			PrintLn("Unknown operation of notifying joining of room from id {}. ({} bytes)", id, bytes);

			delete context;
			break;
		};

		// task_ctx is a SendContext
		task_ctx->SetOperation(OpSend);
		AddSendContext(static_cast<iconer::app::SendContext*>(task_ctx));

		EventOnNotifyRoomJoin(*user);

		PrintLn("User {} joined to room {}.", id, user->GetRoom()->GetID());
	}
	break;

	case OpCreateRoom:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user) UNLIKELY
		{
			PrintLn("Unknown operation of notifying creation of room from id {}. ({} bytes)", id, bytes);

			delete context;
			break;
		};

		// task_ctx is the roomContext of user
		task_ctx->TryChangeOperation(OpCreateRoom, None);

		PrintLn("User {} created a room {}.", id, user->GetRoom()->GetID());
	}
	break;

	case OpReserveRoom:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user) UNLIKELY
		{
			PrintLn("Unknown operation of room creation from id {}. ({} bytes)", id, bytes);

			delete context;
			break;
		}
		else LIKELY
		{
			//PrintLn("User {} is attempting to create a room.", id);
		};

		// task_ctx is the roomContext of user

		const auto room_id = bytes;
		const auto place = roomManager.FindRoom(room_id);

		if (not place.has_value()) UNLIKELY
		{
			PrintLn("Unknown operation of room creation from id {} has no room. ({} bytes)", id, bytes);

			break;
		}
		else if (task_ctx->TryChangeOperation(OpReserveRoom, OpCreateRoom)) LIKELY
		{
			user->SendRoomCreatedPacket(room_id);
		}
		else UNLIKELY
		{
			user->SendFailedCreateRoomPacket(*AcquireSendContext(), iconer::app::RoomContract::InvalidOperation);
		}
	}
	break;

	//case OpR:
	//{

	//}
	//break;

	case OpSignInFailed:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown cancel of signing in from id {} ({} bytes)", id, bytes);

			delete context;
			break;
		}
		else
		{
			PrintLn("User {} has failed to sign in. ({} bytes)", id, bytes);
		}

		// task_ctx is the mainContext of user

		user->BeginClose();
	}
	break;

	case OpAssignID:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown signing in from id {} ({} bytes)", id, bytes);

			delete context;
			break;
		}

		// task_ctx is the mainContext of user

		if (task_ctx->TryChangeOperation(OpAssignID, OpSignIn))
		{
			//std::byte namebuf[iconer::app::Settings::nickNameLength * 2 + 1]{};

			//(void)iconer::util::Serialize(namebuf, user->GetName());

			//const char* nickname = reinterpret_cast<const char*>(namebuf);

			//auto nickname = iconer::util::ToString(user->GetName());

			//char namebuf[iconer::app::Settings::nickNameLength * 2 + 1]{};
			//const wchar_t* nickname_ptr = user->GetName().data();

			//size_t len{};
			//std::mbstate_t state{};
			//auto err = wcsrtombs_s(&len, namebuf, &nickname_ptr, iconer::app::Settings::nickNameLength, &state);

			//std::print("User {} is signed in. (name=", id);

			//fputws(user->GetName().data(), stdout);

			//PrintLn(", {} bytes)", bytes);
		}
		else
		{
			PrintLn("User {} is just signed in, but failed to change state. ({} bytes)", id, bytes);

			task_ctx->SetOperation(OpSignInFailed);
			user->SendFailedSignInPacket(iconer::app::ConnectionContract::SignInFailed);
		}
	}
	break;

	case OpPacketProcess:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown packet processor from id {} ({} bytes)", id, bytes);

			delete context;
			break;
		}
		else
		{
			//PrintLn("Packet processor from id {} ({} bytes)", id, bytes);
		}

		ProcessPackets(*user, static_cast<iconer::app::PacketContext*>(context), bytes);
	}
	break;

	case OpClose:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown closing from id {} ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		user->EndClose();

		PrintLn("User {} is just closed", id);

		// restart
		ReserveUser(*user);
	}
	break;

	case OpSend:
	{
		//delete task_ctx;
		AddSendContext(static_cast<iconer::app::SendContext*>(task_ctx));
	}
	break;

	case OpRecv:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown receive from id {} ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		user->EndReceive(bytes);

		if (0 == bytes)
		{
			PrintLn("User {} is just disconnected.", id);

			user->BeginClose();

			break;
		}

		//PrintLn("User {} received {} bytes.", id, bytes);

		//auto buffer = user->AcquireReceivedData();
		ProcessPacketsInline(*user, bytes);

		if (auto io = user->BeginReceive(); io)
		{
			//PrintLn("User {} restarted receiving.", id);
		}
		else
		{
			// Would trigger OnTaskFailure
			PrintLn("User {} has failed to restart receiving, due to {}.", id, std::to_string(io.error()));

			user->BeginClose();
		}
	}
	break;

	case OpOptainRecvMemory:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown receive memory optaining from id {} ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		if (not user->EndOptainReceiveMemory(true))
		{
			//PrintLn("User {} optained its memory for receiving, but the task has an invalid state.", id);

			// Atomic errors would not be detected by io completion port
			user->BeginClose();

			break;
		}

		if (auto io = StartUser(*user); io)
		{
			//PrintLn("User {} started receiving.", id);
		}
		else
		{
			// Would trigger OnTaskFailure
			PrintLn("User {} has failed to start receiving, due to {}.", id, std::to_string(io.error()));
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

		if (auto io = AcceptUser(*user); io)
		{
			PrintLn("User {} is just accepted.", user->GetID());
		}
		else
		{
			PrintLn("Acceptance of user {} is failed, due to {}.", user->GetID(), std::to_string(io.error()));

			throw "AcceptError!";
		}

		if (auto io = TriggerUser(*user); io)
		{
			//PrintLn("User {} started optaining its memory for receiving.", user->GetID());
		}
		else
		{
			// Would trigger OnTaskFailure
			PrintLn("User {} failed to optain its memory for receiving, due to {}.", user->GetID(), std::to_string(io.error()));
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

		if (auto io = GetListenSocket().BeginAccept(task_ctx, user->GetSocket()); io)
		{
			//PrintLn("User {} is reserved.", id);
		}
		else
		{
			PrintLn("Reservation of user {} is failed, due to {}.", id, std::to_string(io.error()));

			throw "ReserveError!";
		}
	}
	break;

	default:
	{
		PrintLn("[Task({})] id={} ({} bytes)", static_cast<void*>(context), id, bytes);
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
		PrintLn("[Null Task]id={} ({} bytes)", id, bytes);
		return;
	}

	task_ctx->ClearIoStatus();

	using namespace iconer::net;
	using enum iconer::app::TaskCategory;

	switch (task_ctx->myCategory)
	{
	case OpSpreadGameTicket:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown failed game ticket from id {}. ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		EventOnFailedToSpreadGameTickets(*user);
	}
	break;

	case OpEnterRoom:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown failed notifying joining of room from id {}. ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		// task_ctx is a SendContext
		task_ctx->SetOperation(OpSend);
		AddSendContext(static_cast<iconer::app::SendContext*>(task_ctx));

		EventOnFailedToNotifyRoomJoin(*user);
	}
	break;

	case OpCreateRoom:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown failed notifying creation of room from id {}. ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		EventOnFailedToMakeRoom(*user);
	}
	break;

	case OpOptainRecvMemory:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown failed receive memory optaining from id {}. ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		PrintLn("User {} has failed on optaining its memory for receiving. (task={})", id, user->EndOptainReceiveMemory(false));

		user->BeginClose();
	}
	break;

	case OpRecv:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown failed receive from id {}. ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		user->BeginClose();
	}
	break;

	case OpSend:
	{
		//delete task_ctx;
		const auto send_ctx = static_cast<iconer::app::SendContext*>(task_ctx);
		//send_ctx->mySharedBuffer.owner_before();

		auto& shared_buffer = send_ctx->mySharedBuffer;
		shared_buffer.reset();

		AddSendContext(send_ctx);
	}
	break;

	case OpSignIn:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown failed signing in from id {}. ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		user->BeginClose();
	}
	break;

	case OpSignInFailed:
	{
		const auto user = userManager.FindUser(id);

		if (nullptr == user)
		{
			PrintLn("Unknown failed cancel of signing in from id {}. ({} bytes)", id, bytes);

			delete task_ctx;
			break;
		}

		user->BeginClose();
	}
	break;

	default:
	{
		PrintLn("[Failed Task({})] id={} ({} bytes)", static_cast<void*>(context), id, bytes);
	}
	break;
	}
}
