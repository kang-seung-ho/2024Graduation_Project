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
	return room.StartGame() ? Schedule(room, 0) : false;
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
	room.CloseGame();
}
