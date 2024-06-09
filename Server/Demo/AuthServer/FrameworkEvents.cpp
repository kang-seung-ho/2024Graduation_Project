#include "Framework.hpp"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>

#include "IoContext.hpp"

import <cstdlib>;
import <memory>;
import <span>;
import <print>;
import <atomic>;

auth::Server::Result
auth::Server::BeginReceive()
{
	const auto buffer = recvBuffer.get();

	::WSABUF recv_wbuffer
	{
		.len = maxRecvSize,
		.buf = recvBuffer.get()
	};
	::DWORD recv_flags{ 0 };

	const auto& address = *recvAddress;
	::INT sockaddr_length = static_cast<INT>(sizeof(sockaddr_in));

	const auto result = ::WSARecvFrom(serverSocket
		, &recv_wbuffer, 1U
		, &recvBytes
		, &recv_flags
		, reinterpret_cast<SOCKADDR*>(recvAddress), &sockaddr_length
		, recvContext, nullptr);

	if (SOCKET_ERROR == result) UNLIKELY
	{
		const auto error_code = WSAGetLastError();

		if (WSAEWOULDBLOCK == error_code or WSA_IO_PENDING == error_code)
		{
			return Result{};
		}
		else
		{
			return std::unexpected{ error_code };
		}
	}
	else
	{
		return Result{};
	}
}

void
auth::Server::EndReceive()
{
	recvContext->Clear();
}

auth::Server::Result
auth::Server::BeginSend()
{
	const auto sender = new SendContext{};
	sender->Clear();
	sender->myBuffer = std::make_unique<char[]>(recvBytes);

	const auto& send_buffer = sender->myBuffer;
	std::memcpy(send_buffer.get(), recvBuffer.get(), recvBytes);

	::WSABUF send_wbuffer
	{
		.len = recvBytes,
		.buf = send_buffer.get()
	};
	::DWORD sent_bytes{};

	const auto sent = ::WSASendTo(serverSocket
		, &send_wbuffer, 1, &sent_bytes
		, 0
		, reinterpret_cast<const SOCKADDR*>(recvAddress), sizeof(sockaddr_in)
		, sender, nullptr);

	if (SOCKET_ERROR == sent) UNLIKELY
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
		return Result{};
	}
}

void
auth::Server::EndSend(SendContext* context)
{
	if (context != nullptr)
	{
		delete context;
	}
}

void
auth::Server::OnReceived(unsigned long bytes)
{
	recvBytes = bytes;

	BeginSend();

	PrintReceivedData();
}

void
auth::Server::OnSent(SendContext* context)
{
	std::println("Echoing the data...");
}
