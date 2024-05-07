module;
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
