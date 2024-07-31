module;
#include <cstdio>
#include <cstdlib>
#include <print>

#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Framework;
import Iconer.Net.Socket;

void
ServerFramework::Startup()
{
	super::Startup();

	if (auto io = GetListenSocket().Open(); io)
	{
		PrintLn("The listen socket is opened.");
	}
	else
	{
		PrintLn("The listen socket could not be opened, due to {}.", std::to_string(io.error()));

		return;
	}

	PrintLn("Generating {} workers...", myTaskPool.GetMaxWorkerNumber());

	try
	{
		myTaskPool.Startup();
	}
	catch (std::exception& e)
	{
		PrintLn("An error occured when generating workers!\nException: {}", e.what());

		throw;
	}

	PrintLn("Reserving {} users...", userManager.maxUserCount);

	userManager.Startup(GetListenSocket());
	roomManager.Startup();

	PrintLn("Server is started.");

	myTaskPool.Schedule((iconer::net::IoContext*)nullptr, 0, 535334434);

	super::PostStartup();

	char input_buffer[128]{};
	constexpr std::uint32_t input_length = sizeof(input_buffer);

	while (true)
	{
		const auto input = scanf_s("%s", input_buffer, input_length);

		if (EOF != input) LIKELY
		{
			if (input_buffer[0] == 'q')
			{
				break;
			}
		}
	}
}
