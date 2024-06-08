#include "Framework.hpp"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>

import <cstdlib>;
import <memory>;
import <span>;
import <print>;
import <atomic>;

std::expected<void, int>
auth::Server::BeginReceive()
{
	const auto buffer = recvBuffer.get();

	::WSABUF recv_wbuffer
	{
		.len = maxRecvSize,
		.buf = buffer
	};
	::DWORD recv_bytes{};
	::DWORD recv_flags{ 0 };

	const auto& address = *recvAddress;
	::INT sockaddr_length = static_cast<INT>(sizeof(sockaddr_in));

	const auto result = ::WSARecvFrom(serverSocket
		, &recv_wbuffer, 1U
		, &recv_bytes
		, &recv_flags
		, reinterpret_cast<SOCKADDR*>(recvAddress), &sockaddr_length
		, nullptr, nullptr);

	if (SOCKET_ERROR == result) UNLIKELY
	{
		const auto error_code = WSAGetLastError();

		if (WSAEWOULDBLOCK != error_code and WSA_IO_PENDING != error_code)
		{
			return std::unexpected{ error_code };
		}
	};

	std::println("[Receive] {} bytes\n"
		"Address family: {}\n"
		"Address: {}\n"
		"Address port: {}\n"
		"Received data: {}"

		, recv_bytes
		, address.sin_family
		, address.sin_addr.S_un.S_addr
		, ::ntohs(address.sin_port)
		, buffer);

	std::println("Echoing the data.");

	::WSABUF send_wbuffer
	{
		.len = recv_bytes,
		.buf = buffer
	};
	::DWORD sent_bytes{};

	const auto sent = ::WSASendTo(serverSocket
		, &send_wbuffer, 1, &sent_bytes
		, 0
		, reinterpret_cast<const SOCKADDR*>(&address), sockaddr_length
		, nullptr, nullptr);

	if (SOCKET_ERROR == sent) UNLIKELY
	{
		const auto error_code = WSAGetLastError();

		if (WSAEWOULDBLOCK != error_code and WSA_IO_PENDING != error_code)
		{
			std::println(" Error when sending echo data. Error code is {}.", error_code);

			std::memset(buffer, 0, maxRecvSize);

			return std::unexpected{ error_code };
		}
	}
	else
	{
		std::println("Echo data are sent.");
	}

	std::memset(buffer, 0, maxRecvSize);

	return std::expected<void, int>{};
}

void
auth::Server::EndReceive()
{}
