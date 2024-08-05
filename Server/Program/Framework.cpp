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
	iconer::app::Room* const room = user->GetRoom();

	if (nullptr != room)
	{
		if (room->Leave(*user))
		{

		}
	}
}

void
ServerFramework::OnUserJoined(iconer::app::Room* room, iconer::app::User* user, std::int32_t member_cnt)
{
	if (0 < member_cnt)
	{
	}
}

void
ServerFramework::OnUserLeft(iconer::app::Room* room, iconer::app::User* user, std::int32_t member_cnt)
{
	if (0 < member_cnt)
	{
		auto&& pk_data = user->mainContext->GetRoomLeftPacketData();

		room->Foreach([&](iconer::app::User& member)
			{
				iconer::app::SendContext* const ctx = AcquireSendContext();

				member.SendGeneralData(*ctx, pk_data.data(), pk_data.size());
			}
		);
	}
}

void
ServerFramework::OnRoomClosed(iconer::app::Room* room)
{
	PrintLn("Room {} is just closed.", room->GetID());
}
