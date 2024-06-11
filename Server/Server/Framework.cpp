#include "Framework.hpp"
#include "Worker.hpp"

import Iconer.Net;
import <cstdio>;
import <cstdlib>;
import <utility>;
import <print>;

using namespace iconer;
using namespace iconer::net;

std::expected<void, iconer::net::ErrorCode>
Framework::Initialize()
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

	listenSocket = Socket::CreateUdpSocket(SocketCategory::Asynchronous, IpAddressFamily::IPv4);

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

	if (auto io = IoCompletionPort::Create(); io)
	{
		std::println("The iocp is created.");

		completionPort = std::move(io.value());
	}
	else
	{
		std::println("Could not create iocp, due to {}.", std::to_string(io.error()));

		return std::unexpected{ io.error() };
	}

	if (auto io = completionPort.Register(listenSocket, 0); io)
	{
		std::println("The listen socket is registered to the iocp.");
	}
	else
	{
		std::println("The listen socket was not able to be registered to the iocp, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	return {};
}

void
Framework::Startup()
{
	std::println("Generating {} workers...", workerCount);

	try
	{
		size_t index{};
		for (auto& th : myWorkers)
		{
			th = std::jthread
			{
				Worker,
				std::ref(*this),
				++index
			};
		}
	}
	catch (std::exception& e)
	{
		std::println("An error occured when generating workers!\nException: {}", e.what());

		throw;
	}

	workerInitializationSynchronizer.wait();

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
Framework::Cleanup()
{
	for (auto& th : myWorkers)
	{
		th.request_stop();
	}

	listenSocket.Close();

	completionPort.Destroy();

	iconer::net::Cleanup();
}

void
Framework::ArriveWorker()
noexcept
{
	workerInitializationSynchronizer.arrive_and_wait();
}

iconer::net::IoEvent
Framework::AwaitForTask()
noexcept
{
	return completionPort.WaitForIoResult();
}

bool
Framework::ProcessTask(iconer::net::IoEvent task)
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
