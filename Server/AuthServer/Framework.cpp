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

import <cstdlib>;
import <memory>;
import <utility>;
import <string>;
import <string_view>;
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

	myListener = ::WSASocket(AF_INET, SOCK_DGRAM, ::IPPROTO::IPPROTO_UDP, nullptr, 0, WSA_FLAG_OVERLAPPED);
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
		.sin_port = ::ntohs(serverPort),
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

	const auto target = reinterpret_cast<::HANDLE>(myListener);
	const auto port = ::CreateIoCompletionPort(target, iocpHandle, iocpMasterKey, 0);

	if (nullptr == port)
	{
		const auto error_code = ::WSAGetLastError();
		std::println("Error when registering the listen socket to iocp: {}", error_code);

		return std::unexpected{ error_code };
	}

	std::println("The listen socket is registered to the iocp.");

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

	std::println("Generating {} pipelines...", iocpWorkerCount);

	for (auto& pipeline : myPipelines)
	{
		pipeline = Pipeline
		{
			.myContext = std::make_unique<::OVERLAPPED>(),
			.recvBuffer = std::make_unique<char[]>(maxRecvSize),
			.recvOffset = 0
		};
	}

	std::println("Pipelines are generated.");

	std::println("Generating {} workers...", iocpWorkerCount);

	size_t index{ static_cast<size_t>(-1) };
	for (auto& thread : myThreads)
	{
		++index;
		thread = std::jthread
		{
			Worker,
			index,
			std::ref(*this),
			std::ref(myPipelines[index])
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

	NotifyWork();

	while (true)
	{

	}

	iocpBarrier.notify_all();

	return std::expected<void, int>{};
}

void
Framework::ArriveWorkersIntialized()
noexcept
{
	return workerInitializationSynchronizer.arrive_and_wait();
}

void
Framework::AwaitWork()
noexcept
{
	iocpBarrier.wait(false);

	iocpBarrier = false;
}

void
Framework::NotifyWork()
noexcept
{
	iocpBarrier = true;
	return iocpBarrier.notify_one();
}

std::expected<void, int>
Framework::ReceiveFrom(Pipeline& pipeline, sockaddr_in& address)
{
	const auto& recv_context = pipeline.myContext;
	const auto& recv_buffer = pipeline.recvBuffer;
	auto& recv_offset = pipeline.recvOffset;

	::WSABUF recv_wbuffer
	{
		.len = maxRecvSize - recv_offset,
		.buf = recv_buffer.get()
	};
	::DWORD recv_bytes{};
	::DWORD recv_flags{ 0 };

	::INT recv_sockaddr_length = static_cast<INT>(sizeof(sockaddr_in));

	const auto result = ::WSARecvFrom(myListener
		, &recv_wbuffer, 1U
		, &recv_bytes
		, &recv_flags
		, reinterpret_cast<SOCKADDR*>(&address), &recv_sockaddr_length
		, recv_context.get(), nullptr);

	if (SOCKET_ERROR == result)
	{
		const auto error_code = WSAGetLastError();

		if (WSAEWOULDBLOCK == error_code or WSA_IO_PENDING == error_code)
		{
			return std::expected<void, int>{};
		}
		else
		{
			return std::unexpected{ error_code };
		}
	}
	else
	{
		return std::expected<void, int>{};
	}
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
