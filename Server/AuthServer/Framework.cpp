#include "Framework.hpp"
#pragma warning(push)
#pragma warning(disable : 4006)
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma warning(pop)

#include "Worker.hpp"
#include "IoContext.hpp"

import <span>;
import <print>;

constinit std::latch workerInitializationSynchronizer{ auth::iocpWorkerCount };

std::expected<void, int>
auth::Server::Initialize()
{
	std::println("Authenticator server is initiated.");

	::WSADATA version_data{};
	const int startup = ::WSAStartup(MAKEWORD(2, 2), std::addressof(version_data));
	if (0 != startup)
	{
		const auto error_code = ::WSAGetLastError();
		std::println("Error when startup the system: {}", error_code);

		return std::unexpected{ error_code };
	}

	constexpr std::uint32_t flags = WSA_FLAG_OVERLAPPED;
	//constexpr std::uint32_t flags = 0;
	serverSocket = ::WSASocket(AF_INET, SOCK_DGRAM, ::IPPROTO::IPPROTO_UDP, nullptr, 0, flags);
	if (INVALID_SOCKET == serverSocket)
	{
		const auto error_code = ::WSAGetLastError();
		std::println("Error when creating the listen socket: {}", error_code);

		return std::unexpected{ error_code };
	}

	std::println("The listen socket is created.");

	constexpr int reuse_address = true;
	if (0 != ::setsockopt(serverSocket
		, SOL_SOCKET
		, SO_REUSEADDR
		, reinterpret_cast<const char*>(&reuse_address), sizeof(int)))
	{
		const auto error_code = ::WSAGetLastError();
		std::println("Error when setting up the listen socket: {}", error_code);

		return std::unexpected{ error_code };
	}

	const ::SOCKADDR_IN ipv4_sockaddr
	{
		.sin_family = AF_INET,
		.sin_port = ::htons(serverPort),
		.sin_addr = ::in4addr_any,
		.sin_zero{}
	};

	if (0 != ::bind(serverSocket, reinterpret_cast<const SOCKADDR*>(std::addressof(ipv4_sockaddr)), sizeof(ipv4_sockaddr)))
	{
		const auto error_code = ::WSAGetLastError();
		std::println("Error when bindig the listen socket to any address: {}", error_code);

		return std::unexpected{ error_code };
	}

	std::println("The listen socket is established.");
	iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, iocpMasterKey, iocpWorkerCount);

	if (nullptr == iocpHandle)
	{
		const auto error_code = ::WSAGetLastError();
		std::println("Error when creating the iocp: {}", error_code);

		return std::unexpected{ error_code };
	}

	std::println("The iocp is created.");

	//*
	const auto target = reinterpret_cast<::HANDLE>(serverSocket);
	const auto port = ::CreateIoCompletionPort(target, iocpHandle, iocpMasterKey, 0);

	if (nullptr == port)
	{
		const auto error_code = ::WSAGetLastError();
		std::println("Error when registering the listen socket to iocp: {}", error_code);

		return std::unexpected{ error_code };
	}

	std::println("The listen socket is registered to the iocp.");
	//*/

	if (0 != std::atexit(&ExitHandler))
	{
		std::println("Error when registering a exit handler.");

		return std::unexpected{ EXIT_FAILURE };
	}

	if (0 != std::at_quick_exit(&ExitHandler))
	{
		std::println("Error when registering a emergency exit handler.");

		return std::unexpected{ EXIT_FAILURE };
	}

	//recvContext = new IoContext{ IoCategory::Recv };
	//recvContext->Clear();

	recvBuffer = std::make_unique<char[]>(maxRecvSize);
	recvAddress = new SOCKADDR_IN{};

	std::println("Generating workers...", iocpWorkerCount);

	size_t index{ static_cast<size_t>(-1) };

	myThreads[0] = std::jthread
	{
		ReceiveHandler,
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

void
auth::Server::ArriveWorkersIntialized()
noexcept
{
	return workerInitializationSynchronizer.arrive_and_wait();
}
