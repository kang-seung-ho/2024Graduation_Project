#include "Framework.hpp"

import Iconer.App.User;
import Iconer.App.PacketContext;
import Iconer.App.PacketProtocol;

void
ServerFramework::ProcessPacket(iconer::app::User& user)
const
{
	auto buffer = user.GetReceivedData();

	size_t index = 0;
	for (auto& byte : buffer)
	{

	}
}
