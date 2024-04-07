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

bool
demo::Framework::OnStartGame(iconer::app::Room& room)
{
	return true;
}

void
demo::Framework::OnFailedToStartGame(iconer::app::Room& room)
noexcept
{
	using enum iconer::app::RoomStates;

	if (room.TryChangeState(InGame, Closing))
	{
		room.SetOperation(iconer::app::AsyncOperations::OpCloseGame);
		(void)Schedule(room, 0);
	}
	else
	{
		room.TryCancelBeginGame();
	}
}

void
demo::Framework::OnCloseGame(iconer::app::Room& room)
{
	if (room.TryEndClose(0 < room.GetMembersCount()
		? iconer::app::RoomStates::Idle : iconer::app::RoomStates::None))
	{
		room.SetOperation(iconer::app::AsyncOperations::None);
		room.proceedMemberCount = 0;
		room.isGameReadyFailed = false;

		room.ForEach
		(
			[](iconer::app::User& member)
			{
				member.TryChangeState(iconer::app::UserStates::MakingGame, iconer::app::UserStates::InRoom);
				member.TryChangeState(iconer::app::UserStates::ReadyForGame, iconer::app::UserStates::InRoom);
			}
		);
	}
}
