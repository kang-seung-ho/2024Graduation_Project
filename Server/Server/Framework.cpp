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

	if (const auto io = iconer::net::Startup(); io)
	{
		std::println("The network system is Initiated.");
	}
	else
	{
		std::println("Could not initialize network system, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	listenSocket = Socket::CreateTcpSocket(SocketCategory::Asynchronous, IpAddressFamily::IPv4);

	if (listenSocket)
	{
		std::println("The listen socket is created.");
	}
	else
	{
		const auto error = AcquireNetworkError();

		std::println("The listen socket was not created, due to {}.", std::to_string(error));

		return std::unexpected{ error };
	}

	if (const auto io = listenSocket.BindToHost(serverPort); io)
	{
		std::println("The listen socket is bound to host:({}).", serverPort);
	}
	else
	{
		std::println("The listen socket could not be bound to host, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	if (const auto io = listenSocket.ReusableAddress(true); io)
	{
		std::println("The listen socket now would recycle its address. ({})", listenSocket.ReusableAddress());
	}
	else
	{
		std::println("The listen socket could not be set to re-use its address, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}
	
	if (const auto io = listenSocket.SetTcpNoDelay(false); io)
	{
		std::println("The listen socket would not use Nagle algorithm. ({})", listenSocket.IsTcpNoDelay());
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

	if (auto io = myTaskPool.Register(listenSocket, 0); io)
	{
		std::println("The listen socket is registered to the task pool.");
	}
	else
	{
		std::println("The listen socket was not able to be registered to the task pool, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	return {};
}

void
ServerFramework::Startup()
{
	std::println("Generating {} workers...", myTaskPool.workerCount);

	try
	{
		myTaskPool.Startup(*this);
	}
	catch (std::exception& e)
	{
		std::println("An error occured when generating workers!\nException: {}", e.what());

		throw;
	}

	std::println("Server is started.");

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

	listenSocket.Close();

	iconer::net::Cleanup();
}

bool
ServerFramework::ProcessTask(iconer::net::IoEvent task)
{
	if (task.isSucceed) LIKELY
	{

	}
	else UNLIKELY
	{

	}

	packetProcessors;

	return task.isSucceed;
}
