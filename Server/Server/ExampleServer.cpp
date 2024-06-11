#include "Framework.hpp"

import Iconer.Net;
import Iconer.Net.EndPoint;
import Iconer.Net.Socket;
import Iconer.Net.IoCompletionPort;
import <print>;

Framework serverSystem{};

int main()
{
	if (auto init = serverSystem.Initialize(); init)
	{
		std::println("Server is Initiated.");
	}
	else
	{
		std::println("Could not initialize this server, due to {}.", std::to_string(init.error()));
	}

	serverSystem.Startup();

	std::println("Closing server...");

	serverSystem.Cleanup();

	return 0;
}
