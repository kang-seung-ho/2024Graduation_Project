module;
#include <string>
#include <string_view>

#define RETURN_BACK_SENDER(ctx) \
{ \
	auto sender = std::launder(static_cast<BorrowedSendContext*>(ctx)); \
	sender->ReturnToBase(); \
}

module Demo.Framework;
import Iconer.Application.IContext;
import Iconer.Application.BlobSendContext;

using namespace iconer::app;

bool
demo::Framework::RouteEvent(bool is_succeed
	, const std::uint64_t& io_id
	, const ptrdiff_t& io_bytes
	, IContext* ctx)
{
	ctx->Clear();

	using enum AsyncOperations;

	switch (ctx->GetOperation())
	{
		// Phase 0
	case OpReserveSession:
	{
		auto&& user = *std::launder(static_cast<User*>(ctx));
		const IdType& id = user.GetID();

		if (not is_succeed)
		{
			myLogger.LogError(L"\tReserving an acceptance has failed on user {}\n", id);
			OnFailedReservingAccept();
		}
		else if (auto error = OnReserveAccept(user); error.has_value())
		{
			myLogger.LogError(L"\tReserving an acceptance has failed on user {} due to {}\n", id, error.value());
			OnFailedReservingAccept();
		}
		else
		{
			myLogger.Log(L"\tAcceptance is reserved on user {}\n", id);
		}
	}
	break;

	// Phase 1
	// an user is connected
	case OpAccept:
	{
		auto&& user = *std::launder(static_cast<User*>(ctx));
		const IdType& id = user.GetID();

		if (not is_succeed)
		{
			myLogger.LogError(L"\ttConnection has failed on user {}\n", id);
			OnFailedUserConnect(user);
		}
		else if (auto result = OnUserConnected(user); not result.has_value())
		{
			myLogger.LogError(L"\tUser {} is connected, but acceptance has failed due to {}\n", id, result.error());
			OnFailedUserConnect(user);
		}
		else
		{
			myLogger.Log(L"\tUser {} is connected\n", id);
		}
	}
	break;

	// Phase 2
	// received a nickname, and send an id of user
	case OpSignIn:
	{
		auto user = std::launder(static_cast<User*>(ctx));
		const IdType& id = user->GetID();

		if (not is_succeed)
		{
			myLogger.LogError(L"\tSigning In has failed on user {}\n", id);
			OnFailedUserSignIn(*user);
		}
		else if (0 == io_bytes)
		{
			myLogger.LogWarning(L"\tUser {} is just disconnected when they are signing in\n", id);
			OnFailedUserSignIn(*user);
		}
		else if (auto result = OnUserSignedIn(*user, io_bytes); not result.has_value())
		{
			myLogger.LogError(L"\tSigning In has failed on user {} due to {}\n", id, result.error());
			OnFailedUserSignIn(*user);
		}
		else
		{
			myLogger.Log(L"\tUser {} is signed in\n", id);
		}
	}
	break;

	// Phase 3
	// sent an id of user
	case OpAssignID:
	{
		auto user = std::launder(static_cast<User*>(ctx));
		const IdType& id = user->GetID();

		if (not is_succeed)
		{
			myLogger.LogError(L"\tNotifying the id to user {} has failed\n", id);
			OnFailedNotifyId(*user);
		}
		else if (auto result = OnNotifyUserId(*user); not result.has_value())
		{
			myLogger.Log(L"\tThe Id is notified to user {}", id);
			myLogger.LogError(L", but cannot start receiving due to {}\n", result.error());
			OnFailedNotifyId(*user);
		}
		else
		{
			myLogger.Log(L"\tUser {} has been notified id, now start receiving\n", id);
		}
	}
	break;

	// Phase 4~
	case OpRecv:
	{
		auto user = std::launder(static_cast<User*>(ctx));
		const IdType& id = user->GetID();

		if (not is_succeed)
		{
			myLogger.LogError(L"\tReceving has failed on user {}\n", id);
			OnFailedReceive(*user);
		}
		else if (0 == io_bytes)
		{
			myLogger.LogWarning(L"\tUser {} is just disconnected\n", id);
			OnFailedReceive(*user);
		}
		else if (auto error = OnReceived(*user, io_bytes); not error.has_value())
		{
			myLogger.LogError(L"\tReceving has failed on user {} due to {}\n", id, error.error());
			OnFailedReceive(*user);
		}
		else
		{
			myLogger.Log(L"\tReceving is proceed on user {}\n", id);
		}
	}
	break;

	// Phase 4~
	case OpSend:
	{
		if (not is_succeed)
		{
			myLogger.LogError(L"\tSending has failed on user {}\n", io_id);
		}
		else if (0 == io_bytes)
		{
			myLogger.LogError(L"\tSending has failed on user {} due to 0 bytes\n", io_id);
		}
		else
		{
			myLogger.Log(L"\tUser {} has sent {} bytes\n", io_id, io_bytes);
		}

		auto sender = std::launder(static_cast<BlobSendContext*>(ctx));
		delete sender;
	}
	break;

	// Phase 4~
	case OpSendBorrowed:
	{
		RETURN_BACK_SENDER(ctx);
	}
	break;

	// Phase 4~
	case OpDisconnect:
	{
		auto user = std::launder(static_cast<User*>(ctx));
		const IdType& id = user->GetID();

		if (not is_succeed)
		{
			myLogger.LogError(L"\tUser {} has failed to disconnect\n", id);
		}
		else if (auto error = OnUserDisconnected(*user); error.has_value())
		{
			myLogger.LogError(L"\tUser {} would not be disconnected due to {}\n", id, error.value());
		}
		else
		{
			myLogger.Log(L"\tUser {} has been disconnected\n", id);
		}
	}
	break;

	// Phase 5
	case OpReserveRoom:
	{
		auto room = std::launder(static_cast<Room*>(ctx));
		const IdType& room_id = room->GetID();
		const IdType user_id = static_cast<IdType>(io_id);
		auto user = FindUser(user_id);

		if (not is_succeed)
		{
			myLogger.LogError(L"\tUser {} could not reserve room {}\n", user_id, room_id);
			OnFailedToReserveRoom(*room, *user, RoomContract::ServerError);
		}
		else if (auto result = OnReservingRoom(*room, *user); RoomContract::Success != result)
		{
			myLogger.LogError(L"\tUser {} could not reserve room {} due to {}\n", user_id, room_id, result);
			OnFailedToReserveRoom(*room, *user, result);
		}
		else
		{
			myLogger.Log(L"\tUser {} reserved room {}\n", user_id, room_id);
		}
	}
	break;

	// Phase 5
	case OpCreateRoom:
	{
		auto room = std::launder(static_cast<Room*>(ctx));
		const IdType& room_id = room->GetID();
		const IdType user_id = static_cast<IdType>(io_id);
		auto user = FindUser(user_id);

		if (not is_succeed)
		{
			myLogger.LogError(L"\tUser {} could not create a room\n", user_id);
			OnFailedToCreateRoom(*room, *user, RoomContract::ServerError);
		}
		else if (auto result = OnCreatingRoom(*room, *user); RoomContract::Success != result)
		{
			myLogger.LogWarning(L"\tUser {} could not create room at {} due to {}\n", user_id, room_id, result);
			OnFailedToCreateRoom(*room, *user, result);
		}
		else
		{
			myLogger.Log(L"\tUser {} created a room at {}\n", user_id, room_id);
		}
	}
	break;

	// Phase 5
	case OpEnterRoom:
	{
		const IdType user_id = static_cast<IdType>(io_id);
		const IdType room_id = static_cast<IdType>(io_bytes);
		auto user = FindUser(user_id);
		auto room = FindRoom(room_id);

		if (not is_succeed)
		{
			myLogger.LogError(L"\tUser {} could not enter to room {}\n", user_id, room_id);

			OnFailedToJoinRoom(*room, *user, RoomContract::ServerError);
		}
		else if (auto result = OnJoiningRoom(*room, *user); RoomContract::Success != result)
		{
			myLogger.LogWarning(L"\tUser {} could not enter to room {} due to {}\n", user_id, room_id, result);

			OnFailedToJoinRoom(*room, *user, result);
		}
		else
		{
			myLogger.Log(L"\tUser {} entered to room {}\n", user_id, room_id);

			OnSucceedToJoinRoom(*room, *user);
		}
	}
	break;

	// Phase 5
	case OpLeaveRoom:
	{
		const IdType user_id = static_cast<IdType>(io_id);
		auto user = FindUser(user_id);

		ctx->SetOperation(OpSendBorrowed);

		if (not is_succeed)
		{
			myLogger.LogError(L"\tUser {} has failed to send a left packet\n", user_id);
		}
		else if (auto result = OnLeavingRoom(*user); not result)
		{
			myLogger.LogError(L"\tUser {} has failed to left the room\n", user_id);
		}
		else
		{
			myLogger.Log(L"\tUser {} has been left from the room\n", user_id);
		}

		RETURN_BACK_SENDER(ctx);
	}
	break;

	// Phase 5
	case OpCloseRoom:
	{
		auto room = std::launder(static_cast<Room*>(ctx));

		if (nullptr != room)
		{
			const IdType& room_id = room->GetID();
			OnClosingRoom(*room);

			if (not is_succeed)
			{
				myLogger.LogError(L"\tRoom {}'s operation that closes room is failed\n", room_id);
			}
			else
			{
				myLogger.Log(L"\tRoom {} has been closed\n", room_id);
			}
		}
		else
		{
			myLogger.LogError(L"\tInvalid room closing operation is detected\n");
		}
	}
	break;

	// Phase 6
	case OpNotifyMember:
	{
		const IdType user_id = static_cast<IdType>(io_id);
		auto user = FindUser(user_id);

		if (not is_succeed)
		{
			myLogger.LogError(L"\tUser {}'s operation that notifies members in the room has been failed\n", user_id);

			OnFailedNotifyRoomMember(*user);
		}
		else if (auto error = OnNotifyMemberOfRoom(*user); not error.has_value())
		{
			myLogger.LogError(L"\tUser {} has failed to notify members in the room, due to {}\n", user_id, error.error());
		}
		else
		{
			myLogger.Log(L"\tUser {} has been notified members in the room\n", user_id);
		}
	}
	break;

	// Phase 6
	case OpNotifyRoom:
	{
		const IdType user_id = static_cast<IdType>(io_id);
		auto user = FindUser(user_id);

		if (not is_succeed)
		{
			myLogger.LogError(L"\tUser {}'s operation that notifies rooms has been failed\n", user_id);
		}
		else if (auto error = OnNotifyRoomsList(*user); not error.has_value())
		{
			myLogger.LogError(L"\tUser {} has failed to send a list of rooms due to {}\n", user_id, error.error());
		}
		else
		{
			myLogger.Log(L"\tUser {} sent a list of rooms\n", user_id);
		}
	}
	break;

	// Phase 7
	case OpNotifyTeam:
	{
		const IdType user_id = static_cast<IdType>(io_id);
		auto user = FindUser(user_id);

		if (not is_succeed)
		{
			myLogger.LogError(L"\tUser {}'s operation that notifies its team has been failed\n", user_id);
		}
		else if (not OnNotifyTeam(*user))
		{
			myLogger.LogError(L"\tUser {}'s operation that notifies its team is failed\n", user_id);
		}
		else
		{
			myLogger.Log(L"\tUser {} broadcasted its team\n", user_id);
		}
	}
	break;

	// Phase 7
	case OpCreateGame:
	{
		const IdType user_id = static_cast<IdType>(io_id);
		auto user = FindUser(user_id);

		if (not is_succeed)
		{
			myLogger.LogError(L"\tUser {}'s operation that makes a game has been failed\n", user_id);

			OnFailedToCreateGame(*user);
		}
		else if (not OnCreateGame(*user))
		{
			myLogger.LogError(L"\tUser {}'s operation that makes a game is failed\n", user_id);
		}
		else
		{
			myLogger.Log(L"\tUser {} made a game\n", user_id);
		}
	}
	break;

	// Phase 7
	case OpSpreadGameTicket:
	{
		auto room = std::launder(static_cast<Room*>(ctx));

		if (not is_succeed)
		{
			myLogger.Log(L"\tRoom {} has failed to send some game tickets\n", room->GetID());

			OnFailedToBroadcastGameTickets(*room);
		}
		else if (not OnBroadcastGameTickets(*room))
		{
			myLogger.Log(L"\tRoom {} could not send some game tickets\n", room->GetID());
		}
		else
		{
			myLogger.Log(L"\tRoom {} broadcasted game tickets\n", room->GetID());
		}
	}
	break;

	// Phase 7
	case OpGameTicketing:
	{
		const IdType user_id = static_cast<IdType>(io_id);
		auto user = FindUser(user_id);

		ctx->SetOperation(OpSendBorrowed);

		if (not is_succeed)
		{
			myLogger.Log(L"\tUser {} has failed to send a game ticket\n", user_id);

			OnFailedToSendGameTicket(*user);
		}
		else if (not OnSentGameTicket(*user))
		{
			myLogger.Log(L"\tUser {} could not send a game ticket\n", user_id);
		}
		else
		{
			myLogger.Log(L"\tUser {} sent a game ticket\n", user_id);
		}

		RETURN_BACK_SENDER(ctx);
	}
	break;

	// Phase 7
	case OpReadyGame:
	{
		const IdType user_id = static_cast<IdType>(io_id);
		auto user = FindUser(user_id);

		ctx->SetOperation(OpSendBorrowed);

		if (not is_succeed)
		{
			myLogger.LogError(L"\tUser {}'s operation of preparing the game has failed\n", user_id);

			OnFailedToLoadGame(*user);
		}
		else if (not OnGameIsLoaded(*user))
		{
			myLogger.Log(L"\tThe game is not loaded on user {}\n", user_id);
		}
		else
		{
			myLogger.Log(L"\tUser {} prepared the game\n", user_id);
		}

		RETURN_BACK_SENDER(ctx);
	}
	break;

	// Phase 7
	case OpStartGame:
	{
		auto room = std::launder(static_cast<Room*>(ctx));

		if (not is_succeed)
		{
			myLogger.LogError(L"\tRoom {}'s operation of starting the game has failed\n", room->GetID());

			OnFailedToStartGame(*room);
		}
		else if (not OnStartGame(*room))
		{
			myLogger.Log(L"\tCannot start a game at room {}\n", room->GetID());

			OnFailedToStartGame(*room);
		}
		else
		{
			myLogger.Log(L"\tThe game is started at room {}\n", room->GetID());
		}
	}
	break;

	// Phase 7~
	case OpLeaveGame:
	{
		auto user = std::launder(static_cast<User*>(ctx));
		const IdType& id = user->GetID();

		// TODO
	}
	break;

	// Phase 7~
	case OpCloseGame:
	{
		auto room = std::launder(static_cast<Room*>(ctx));

		OnCloseGame(*room);
	}
	break;

	// Phase 8
	case OpCreateCharacters:
	{
		auto room = std::launder(static_cast<Room*>(ctx));

		if (not is_succeed)
		{
			myLogger.LogError(L"\tRoom {}'s operation of creating characters has failed\n", room->GetID());

			OnFailedToCreateCharacters(*room);
		}
		else if (not OnCreatingCharacters(*room))
		{
			myLogger.Log(L"\tRoom {} cannot create characters\n", room->GetID());

			OnFailedToCreateCharacters(*room);
		}
		else
		{
			myLogger.Log(L"\tCharacters are created in room {}\n", room->GetID());
		}
	}
	break;

	// Phase 8
	case OpUpdateRoom:
	{
		auto room = std::launder(static_cast<Room*>(ctx));

		if (not is_succeed)
		{
			myLogger.LogError(L"\tRoom {}'s operation of update has failed\n", room->GetID());

			OnFailedToUpdateRoom(*room);
		}
		else if (not OnUpdatingRoom(*room))
		{
			myLogger.Log(L"\tCannot update the room {}\n", room->GetID());

			OnFailedToUpdateRoom(*room);
		}
		else
		{
			myLogger.Log(L"\tRoom {} is updated\n", room->GetID());
		}
	}
	break;

	// Phase 8
	case OpCheckGuardian:
	{
		auto room = std::launder(static_cast<Room*>(ctx));

	}
	break;

	// Phase 8
	case OpCheckDead:
	{
		auto room = std::launder(static_cast<Room*>(ctx));

	}
	break;

	// Phase 8
	case OpRpc:
	{
		if (not is_succeed)
		{
			myLogger.LogError(L"\tRPC has failed at room {}\n", io_id);
		}
		else if (not OnRpc(ctx, static_cast<IdType>(io_id)))
		{
			myLogger.Log(L"\tRPC is failed at room {}\n", io_id);
		}
		else
		{
			myLogger.Log(L"\tRPC is proceed at room {}\n", io_id);
		}
	}
	break;

	// Phase 8
	case OpSendRpc:
	{
		OnSentRpc(ctx);
	}
	break;

	// Phase 8
	case OpCleanRpc:
	{
		OnCleanRpc(ctx);
	}
	break;

	// Extras Phase
	case OpEndWorkers:
	{
		return false;
	}

	default:
	{
		if (not is_succeed)
		{
			myLogger.LogError(L"\tUnknown operation '{}' has failed on {}\n", static_cast<int>(ctx->GetOperation()), reinterpret_cast<std::uintptr_t>(ctx));
		}
		else
		{
			myLogger.Log(L"\tAn unknown operation '{}' has been executed on {}\n", static_cast<int>(ctx->GetOperation()), reinterpret_cast<std::uintptr_t>(ctx));
		}
	}
	break;
	}

	return true;
}
