module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Framework;
import Iconer.App.TaskContext;
import Iconer.App.SendContext;

void
ServerFramework::EventOnUserList(iconer::app::User& user, std::byte* data)
{
	const auto room = user.myRoom.load();

	if (nullptr != room)
	{
		user.SendGeneralData(*AcquireSendContext(), room->MakeMemberListPacket());
	}
}
