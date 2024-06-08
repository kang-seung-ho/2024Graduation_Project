#include "Framework.hpp"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "IoContext.hpp"
#include "Worker.hpp"

import <span>;
import <print>;

std::expected<void, int>
Framework::Initialize()
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

	//constexpr std::uint32_t flags = WSA_FLAG_OVERLAPPED;
	constexpr std::uint32_t flags = 0;
	myListener = ::WSASocket(AF_INET, SOCK_DGRAM, ::IPPROTO::IPPROTO_UDP, nullptr, 0, flags);
	if (INVALID_SOCKET == myListener)
	{
		const auto error_code = ::WSAGetLastError();
		std::println("Error when creating the listen socket: {}", error_code);

		return std::unexpected{ error_code };
	}

	std::println("The listen socket is created.");

	constexpr int reuse_address = true;
	if (0 != ::setsockopt(myListener
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

	if (0 != ::bind(myListener, reinterpret_cast<const SOCKADDR*>(std::addressof(ipv4_sockaddr)), sizeof(ipv4_sockaddr)))
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

	/*
	const auto target = reinterpret_cast<::HANDLE>(myListener);
	const auto port = ::CreateIoCompletionPort(target, iocpHandle, iocpMasterKey, 0);

	if (nullptr == port)
	{
		const auto error_code = ::WSAGetLastError();
		std::println("Error when registering the listen socket to iocp: {}", error_code);

		return std::unexpected{ error_code };
	}

	std::println("The listen socket is registered to the iocp.");
	*/

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

	recvAddress = new SOCKADDR_IN{};

	recvBuffer = std::make_unique<char[]>(maxRecvSize);

	recvBarrier = 0;

	std::println("Generating workers...", iocpWorkerCount);

	size_t index{ static_cast<size_t>(-1) };
	for (auto& thread : myThreads)
	{
		++index;
		thread = std::jthread
		{
			Worker,
			index,
			std::ref(*this)
		};
	}

	workerInitializationSynchronizer.wait();

	std::println("Workers are generated.");

	return std::expected<void, int>{};
}

std::expected<void, int>
Framework::Startup()
{
	std::println("Authenticator server is started.");

	if (auto first_recv = BeginReceive(); not first_recv)
	{
		std::println("A receive error occured, error code: {}", first_recv.error());

		return std::unexpected{ first_recv.error() };
	}

	NotifyWork();

	while (true)
	{
	}

	iocpBarrier.notify_all();

	return std::expected<void, int>{};
}

void
Framework::Cleanup()
{
	if (0 != ::WSACleanup())
	{
		const auto error_code = ::WSAGetLastError();
		std::println("Error: {}", error_code);

		return;
	}

	std::println("Authenticator server is terminated.");
}
