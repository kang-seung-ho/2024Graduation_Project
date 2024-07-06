module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Framework;
import Iconer.App.User;
import Iconer.App.Room;
import <print>;

void
ServerFramework::OnUserDisconnected(iconer::app::User* user)
{
	const auto room = user->myRoom.load();

	if (nullptr != room)
	{
		room->Leave(*user);
	}
}

void
ServerFramework::OnRoomClosed(iconer::app::Room* room)
{
	std::println("Room {} is just closed.", room->GetID());

	room->Foreach([](iconer::app::User& user) {

		}
	);
}
