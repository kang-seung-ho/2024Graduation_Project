#include "Framework.hpp"
#pragma warning(push)
#pragma warning(disable : 4006)
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#pragma warning(pop)

#include "Worker.hpp"

import <type_traits>;
import <span>;
import <print>;

std::latch auth::Server::workerInitializationSynchronizer{ auth::iocpWorkerCount };

std::expected<void, int>
auth::Server::Initialize()
{
	if (auto job = InitializeNetwork(); not job)
	{
		std::println("Error when startup the system: {}", job.error());

		return std::move(job);
	}
	else
	{
		std::println("Authenticator server is initiated.");
	}

	if (auto job = InitializeSockets(); not job)
	{
		std::println("Error when startup the system: {}", job.error());

		return std::move(job);
	}
	else
	{
		std::println("Sockets are established.");
	}
	
	if (auto job = InitializeIocp(); not job)
	{
		std::println("Error when creating the iocp: {}", job.error());

		return std::move(job);
	}
	else
	{
		std::println("The iocp is created.");
	}

	if (auto job = RegisterSocket(serverSocket, iocpMasterKey); not job)
	{
		std::println("Error when registering the listen socket to iocp: {}", job.error());

		return std::move(job);
	}
	else
	{
		std::println("The listen socket is registered to the iocp.");
	}

	if (not InitializeBuffers())
	{
		std::println("Error when creating buffers.");

		return std::unexpected{ 0 };
	}

	std::println("Generating workers...", iocpWorkerCount);

	size_t index{ static_cast<size_t>(-1) };

	myThreads[0] = std::jthread
	{
		Worker,
		++index,
		std::ref(*this)
	};

	myThreads[1] = std::jthread
	{
		DatabaseHandler,
		std::ref(*this)
	};

	myThreads[2] = std::jthread
	{
		Worker,
		++index,
		std::ref(*this)
	};

	myThreads[3] = std::jthread
	{
		Worker,
		++index,
		std::ref(*this)
	};

	workerInitializationSynchronizer.wait();

	std::println("Workers are generated.");

	if (not InitializeExitHandlers())
	{
		std::println("Error when registering a exit handler.");

		return std::unexpected{ 0 };
	}

	return std::expected<void, int>{};
}

std::expected<void, int>
auth::Server::Startup()
{
	std::println("Authenticator server is started.");

	char command[256]{};
	constexpr unsigned cmd_size = sizeof(command);

	while (true)
	{
		auto input = ::scanf_s("%s", command, cmd_size);
		if (EOF != input)
		{
			if (command[0] == 'q')
			{
				break;
			}
		}
	}

	for (auto& worker : myThreads)
	{
		worker.request_stop();
	}

	return std::expected<void, int>{};
}

void
auth::Server::Cleanup()
{
	if (0 == ::CloseHandle(std::exchange(iocpHandle, nullptr))) UNLIKELY
	{
		const auto error_code = ::WSAGetLastError();

		std::println("Error when closing the iocp: {}", error_code);
	}

	if (0 != ::shutdown(serverSocket, SD_BOTH)) UNLIKELY
	{
		const auto error_code = ::WSAGetLastError();

		std::println("Error when shutdowing the socket: {}", error_code);
	}

	if (0 != ::closesocket(serverSocket)) UNLIKELY
	{
		const auto error_code = ::WSAGetLastError();

		std::println("Error when closing the socket: {}", error_code);
	}

	if (0 != ::WSACleanup()) UNLIKELY
	{
		const auto error_code = ::WSAGetLastError();

		std::println("Error when closing the socket system: {}", error_code);
	}

	std::println("Authenticator server is terminated.");
}

auth::Server::Result
auth::Server::RegisterSocket(std::uintptr_t socket, std::uintptr_t key)
noexcept
{
	const auto target = reinterpret_cast<::HANDLE>(socket);

	const auto port = ::CreateIoCompletionPort(target, iocpHandle, key, 0);

	if (nullptr != port)
	{
		return Result{};
	}
	else
	{
		return std::unexpected{ ::WSAGetLastError() };
	}
}

void
auth::Server::PrintReceivedData()
const
{
	std::println("[Receive] {} bytes\n"
		"Address: [{}]{}:{}\n"
		"Received data: {}"

		, recvBytes
		, recvAddress->sin_family
		, recvAddress->sin_addr.S_un.S_addr
		, ::ntohs(recvAddress->sin_port)
		, recvBuffer.get());
}
