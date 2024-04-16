module;
#include <string_view>

#define SEND(user_var, method, ...) \
auto [io, ctx] = ((user_var).method)(__VA_ARGS__); \
if (not io) \
{ \
	ctx.Complete(); \
}
#define SEND_RESULT(captures, user_var, method, ...) \
[captures](){ \
	auto [io, ctx] = ((user_var).method)(__VA_ARGS__); \
	if (not io) \
	{ \
		ctx.Complete(); \
		return false; \
	} \
	return true; \
}()
module Demo.Framework;

using namespace iconer::app;

bool
demo::Framework::OnStartGame(Room& room)
{
	if (room.CanStartGame() and room.TryBeginGame())
	{
		room.ForEach
		(
			[](User& member)
			{
				member.SetState(UserStates::InGame);
				SEND(member, SendGameJustStartedPacket);
			}
		);

		room.SetOperation(AsyncOperations::OpCreateCharacters);
		room.proceedMemberCount = 0;

		return Schedule(room, 0);
	}
	else
	{
		return false;
	}
}

void
demo::Framework::OnFailedToStartGame(Room& room)
noexcept
{
	using enum RoomStates;

	if (room.TryChangeState(InGame, Closing))
	{
		room.SetOperation(AsyncOperations::OpCloseGame);
		(void)Schedule(room, 0);
	}
	else
	{
		room.TryCancelBeginGame();
	}
}

void
demo::Framework::OnCloseGame(Room& room)
{
	if (room.TryEndClose(0 < room.GetMembersCount()
		? RoomStates::Idle : RoomStates::None))
	{
		room.SetOperation(AsyncOperations::None);
		room.proceedMemberCount = 0;
		room.isGameReadyFailed = false;

		room.ForEach
		(
			[](User& member)
			{
				member.TryChangeState(UserStates::MakingGame, UserStates::InRoom);
				member.TryChangeState(UserStates::ReadyForGame, UserStates::InRoom);
			}
		);
	}
}
