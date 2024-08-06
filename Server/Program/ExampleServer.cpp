module Iconer.Framework;
import Iconer.Net;
import Iconer.Net.EndPoint;
import Iconer.Net.Socket;
import Iconer.Net.IoCompletionPort;
import <print>;

ServerFramework serverSystem{};

int main()
{
	if (auto init = serverSystem.Initialize(); init)
	{
		PrintLn("Server is Initiated.");
	}
	else
	{
		PrintLn("Could not initialize this server, due to {}.", std::to_string(init.error()));
	}

	serverSystem.Startup();

	PrintLn("Closing server...");

	serverSystem.Cleanup();

	return 0;
}
