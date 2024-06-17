#include "Framework.hpp"

import Iconer.Net;

void
ServerFramework::Cleanup()
{
	myTaskPool.Cleanup();
	userManager.Cleanup();

	super::Cleanup();

	iconer::net::Cleanup();
}
