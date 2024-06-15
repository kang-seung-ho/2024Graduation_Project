#include "Framework.hpp"

import Iconer.Net;
import <cstdio>;
import <cstdlib>;
import <utility>;
import <print>;

using namespace iconer;
using namespace iconer::net;

std::expected<void, iconer::net::ErrorCode>
ServerFramework::Initialize()
{
	std::println("Starting server...");

	if (const auto io = super::Initialize(); not io)
	{
		std::println("Could not initialize network system, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	if (const auto io = super::listenSocket.BindToHost(serverPort); io)
	{
		std::println("The listen socket is bound to host:({}).", serverPort);
	}
	else
	{
		std::println("The listen socket could not be bound to host, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	if (const auto io = super::listenSocket.ReusableAddress(true); io)
	{
		std::println("The listen socket now would recycle its address. ({})", super::listenSocket.ReusableAddress());
	}
	else
	{
		std::println("The listen socket could not be set to re-use its address, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}
	
	if (const auto io = super::listenSocket.SetTcpNoDelay(false); io)
	{
		std::println("The listen socket would not use Nagle algorithm. ({})", super::listenSocket.IsTcpNoDelay());
	}
	else
	{
		std::println("The listen socket could not set Nagle algorithm, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	if (auto io = myTaskPool.Initialize(); io)
	{
		std::println("The task pool is created.");
	}
	else
	{
		std::println("Could not create task pool, due to {}.", std::to_string(io.error()));

		return std::unexpected{ io.error() };
	}

	if (auto io = myTaskPool.Register(super::listenSocket, 0); io)
	{
		std::println("The listen socket is registered to the task pool.");
	}
	else
	{
		std::println("The listen socket was not able to be registered to the task pool, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	if (userManager.Initialize(myTaskPool.ioCompletionPort))
	{
		return {};
	}
	else
	{
		std::println("The user manager has failed to initialize.");

		return std::unexpected{ ErrorCode::WSAEUSERS };
	}
}

void
ServerFramework::Startup()
{
	super::Startup();

	if (auto io = listenSocket.Open(); io)
	{
		std::println("The listen socket is opened.");
	}
	else
	{
		std::println("The listen socket could not be opened, due to {}.", std::to_string(io.error()));

		return;
	}

	std::println("Generating {} workers...", myTaskPool.GetMaxWorkerNumber());

	try
	{
		myTaskPool.Startup();
	}
	catch (std::exception& e)
	{
		std::println("An error occured when generating workers!\nException: {}", e.what());

		throw;
	}

	std::println("Reserving {} users...", userManager.maxUserCount);

	userManager.Startup(listenSocket);

	std::println("Server is started.");

	myTaskPool.Schedule((IoContext*)nullptr, 0, 535334434);

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

void
ServerFramework::Cleanup()
{
	myTaskPool.Cleanup();
	userManager.Cleanup();

	super::Cleanup();

	iconer::net::Cleanup();
}
