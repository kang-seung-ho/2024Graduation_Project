module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Framework;
import Iconer.App.User;
import Iconer.App.Room;

void
ServerFramework::OnUserDisconnected(iconer::app::User* user)
{
	const auto room = user->myRoom.load();

	if (nullptr != room)
	{
		room->Leave(*user);
	}
}
