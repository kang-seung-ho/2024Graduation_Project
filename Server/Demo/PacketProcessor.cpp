module;
#include <atomic>

#define SEND(user_var, method, ...)\
if (auto [io, ctx] = ((user_var).method)(__VA_ARGS__); not io)\
{\
	ctx.Complete(); \
}

#define IGNORE_DISCARDED_BEGIN \
__pragma (warning(push)) \
__pragma (warning(disable : 4834)) \

#define IGNORE_DISCARDED_END \
__pragma (warning(pop))

module Demo.PacketProcessor;
import Iconer.Utility.Chronograph;
import Iconer.Application.RoomContract;
import Iconer.Application.GameContract;
import Iconer.Application.User;
import Iconer.Application.Room;
import Demo.Framework;

using namespace iconer::app;

void
demo::OnSignOut(User& user)
{
	if (not user.BeginClose())
	{
		user.Cleanup();
	}
}

void
demo::OnRequestVersion(Framework& framework, User& user)
{
	if (user.GetState() != UserStates::None)
	{
		SEND(user, SendRespondVersionPacket);
	}
}

void
demo::OnRequestRoomList(Framework& framework, User& user)
{
	if (user.GetState() != UserStates::None)
	{
		(void)framework.Schedule(user.requestContext, user.GetID());
	}
}

void
demo::OnRequestMemberList(Framework& framework, User& user)
{
	if (user.GetState() != UserStates::None)
	{
		(void)framework.Schedule(user.requestMemberContext, user.GetID());
	}
}

void
demo::OnCreateRoom(demo::Framework& framework, User& user, const wchar_t(&room_title)[16])
{
	bool success = false;
	for (Room*& room : framework.everyRoom)
	{
		if (room->TryReserveContract())
		{
			room->SetOperation(AsyncOperations::OpReserveRoom);

			if (framework.Schedule(room, user.GetID()))
			{
				room->SetName(room_title);
				success = true;
				break;
			}
			else
			{
				if (room->TryCancelContract())
				{
					room->SetOperation(AsyncOperations::None);
				}
			}
		}
	}

	if (not success)
	{
		// every room is busy
		SEND(user, SendRoomCreationFailedPacket, RoomContract::CannotLocateEmptyRoom);
	}
}

void
demo::OnJoinRoom(demo::Framework& framework, User& user, const std::int32_t& room_id)
{
	if (Room* room = framework.FindRoom(room_id); nullptr != room)
	{
		IGNORE_DISCARDED_BEGIN;
		if (user.TryChangeState(UserStates::Idle, UserStates::EnteringRoom))
		{
			iconer::util::Chronograph chronograph{};
			static constexpr auto time_limit = iconer::util::Chronograph::Seconds(3);

			while (true)
			{
				if (RoomStates rstate = room->GetState(); rstate == RoomStates::Idle)
				{
					if (user.myRoomId == -1)
					{
						user.roomContext.SetOperation(AsyncOperations::OpEnterRoom);

						if (not framework.Schedule(user.roomContext, user.GetID(), static_cast<unsigned long>(room_id)))
						{
							// rollback
							user.TryChangeState(UserStates::EnteringRoom, UserStates::Idle);
							user.roomContext.SetOperation(AsyncOperations::None);

							// server error
							SEND(user, SendRoomJoinFailedPacket, RoomContract::ServerError);
						}

						break; // while (true)
					}
					else
					{
						// rollback
						user.TryChangeState(UserStates::EnteringRoom, UserStates::Idle);

						SEND(user, SendRoomJoinFailedPacket, RoomContract::AnotherRoomIsAlreadyAssigned);

						break; // while (true)
					}
				}

				if (chronograph.HasTimePassed(time_limit))
				{
					// rollback
					user.TryChangeState(UserStates::EnteringRoom, UserStates::Idle);

					SEND(user, SendRoomJoinFailedPacket, RoomContract::RoomIsBusy);

					break; // while (true)
				}
			}
		}
		else
		{
			SEND(user, SendRoomJoinFailedPacket, RoomContract::InvalidOperation);
		}
		IGNORE_DISCARDED_END;
	}
	else
	{
		// cannot find a room with the id
		SEND(user, SendRoomJoinFailedPacket, RoomContract::NoRoomFoundWithId);
	}
}

void
demo::OnLeaveRoom(demo::Framework& framework, User& user)
{
	if (user.myRoomId != -1)
	{
		// Send the packet and produce a event
		SEND(user, SendRoomLeftPacket, user.GetID(), true);
	}
}

void
demo::OnGameStartSignal(demo::Framework& framework, User& user)
{
	if (not framework.Schedule(user.gameContext, user.GetID()))
	{
		// cannot prepare the game: server error
		SEND(user, SendCannotStartGamePacket, GameContract::ServerError);
	}
}

struct Remover final : public Room::MemberRemover
{
	using Super = Room::MemberRemover;

	constexpr Remover(demo::Framework& framework) noexcept
		: Super(), myFramework(framework)
	{}

	void operator()(volatile Room& room, const size_t& members_count) const noexcept override
	{
		if (0 == members_count)
		{
			if (room.TryBeginClose(RoomStates::Ready))
			{
				room.SetOperation(AsyncOperations::OpCloseRoom);

				if (not myFramework.Schedule(room, room.GetID()))
				{
					room.Cleanup();
				}
			}
		}
	}

	demo::Framework& myFramework;
};

void
demo::OnGameLoadedSignal(demo::Framework& framework, User& user)
{
	if (not framework.Schedule(user.loadingContext, user.GetID()))
	{
		// cannot prepare the game: server error
		SEND(user, SendCannotStartGamePacket, GameContract::ServerError);
	}
}

void
demo::OnTeamChanged(demo::Framework& framework, User& user, bool is_red_team)
{
	const auto room_id = user.myRoomId.Load();
	if (room_id != -1)
	{
		if (Room* room = framework.FindRoom(room_id); nullptr != room)
		{
			const auto team_id = user.myTeamId.Load(std::memory_order_acquire);
			const auto target = is_red_team ? Team::Red : Team::Blue;

			if (team_id != target and user.myTeamId.CompareAndSet(team_id, target, std::memory_order_release))
			{
				room->Dirty(true);
				framework.SetRoomModifiedFlag();
			}

			(void)framework.Schedule(user.teamChangerContext, user.GetID());
		}
	}
}

void
demo::OnReceivePosition(demo::Framework& framework, User& user, float x, float y, float z)
{
	user.Positions(x, y, z);

	const auto room_id = user.myRoomId.Load();
	if (room_id != -1)
	{
		if (Room* room = framework.FindRoom(room_id); nullptr != room)
		{
			room->ForEach(
				[&user, x, y, z](User& member)
				{
					SEND(member, SendPositionPacket, user.GetID(), x, y, z);
				}
			);
		}
	}
}

void
demo::OnReceiveRotation(Framework& framework, User& user, float roll, float yaw, float pitch)
{
	user.RotationRight(roll);
	user.RotationUp(yaw);
	user.RotationLook(pitch);

	const auto room_id = user.myRoomId.Load();
	if (room_id != -1)
	{
		if (Room* room = framework.FindRoom(room_id); nullptr != room)
		{
			room->ForEach(
				[&user, roll, yaw, pitch](User& member)
				{
					SEND(member, SendRotationPacket, user.GetID(), roll, yaw, pitch);
				}
			);
		}
	}
}

void
demo::OnRpc(Framework& framework, const User& user, RpcProtocol cat, std::int64_t arg0, std::int32_t arg1)
{
	const auto room_id = user.myRoomId.Load();
	if (room_id != -1)
	{
		if (Room* room = framework.FindRoom(room_id); nullptr != room)
		{
			auto ctx = new RpcContext{ cat, room_id, arg0, arg1 };
			(void)framework.Schedule(ctx, user.GetID());
		}
	}
}

void
demo::OnOldRpc(Framework& framework, User& user, std::string&& rpc, long long arg)
{
	const auto room_id = user.myRoomId.Load();
	if (room_id != -1)
	{
		if (Room* room = framework.FindRoom(room_id); nullptr != room)
		{
			room->ForEach(
				[&user, &rpc, arg](User& member)
				{
					SEND(member, SendOldRpcPacket, user.GetID(), rpc, arg);
				}
			);
		}
	}
}
