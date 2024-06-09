#include "Framework.hpp"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "IoContext.hpp"

import <print>;

auth::Server::Result
auth::Server::InitializeNetwork()
{
	::WSADATA version_data{};
	const int startup = ::WSAStartup(MAKEWORD(2, 2), std::addressof(version_data));
	if (0 != startup)
	{
		const auto error_code = ::WSAGetLastError();

		return std::unexpected{ error_code };
	}

	return Result{};
}

auth::Server::Result
auth::Server::InitializeSockets()
{
	constexpr std::uint32_t flags = WSA_FLAG_OVERLAPPED;

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

	return Result{};
}

auth::Server::Result
auth::Server::InitializeIocp()
{
	iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, iocpMasterKey, iocpWorkerCount);

	if (nullptr == iocpHandle)
	{
		return std::unexpected{ ::WSAGetLastError() };
	}

	return Result{};
}

bool
auth::Server::InitializeBuffers()
{
	recvContext = new IoContext{ IoCategory::Recv };
	recvContext->Clear();

	recvBuffer = std::make_unique<char[]>(maxRecvSize);
	recvAddress = new SOCKADDR_IN{};

	return true;
}

auth::Server::Result
auth::Server::InitializeWorkers()
{
	return Result();
}

bool
auth::Server::InitializeExitHandlers()
{
	if (0 != std::atexit(&ExitHandler))
	{
		return false;
	}

	if (0 != std::at_quick_exit(&ExitHandler))
	{
		std::println("Error when registering a emergency exit handler.");

		return false;
	}

	return true;
}

void
auth::Server::ArriveWorkersIntialized()
noexcept
{
	return workerInitializationSynchronizer.arrive_and_wait();
}

void
auth::DatabaseHandler(std::stop_token&& canceller, auth::Server& instance)
{
	std::println("Database worker is generated.");

	instance.ArriveWorkersIntialized();

	while (true)
	{
		if (canceller.stop_requested()) UNLIKELY
		{
			goto finished;
		};


	}

finished:
	std::println("Database worker is terminated.");
}
