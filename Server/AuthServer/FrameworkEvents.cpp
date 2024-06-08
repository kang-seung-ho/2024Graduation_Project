#include "Framework.hpp"
#pragma warning(push)
#pragma warning(disable : 4006)
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#pragma warning(pop)

#include "IoContext.hpp"

import <cstdlib>;
import <memory>;
import <span>;
import <print>;
import <atomic>;

std::expected<void, int>
Framework::BeginReceive()
{
	const auto& address = recvAddress;

	auto recv_buffer = std::make_unique<char[]>(maxRecvSize);
	const auto buffer = recv_buffer.get();

	::WSABUF recv_wbuffer
	{
		.len = maxRecvSize,
		.buf = buffer
	};
	::DWORD recv_bytes{};
	::DWORD recv_flags{ 0 };

	::INT sockaddr_length = static_cast<INT>(sizeof(sockaddr_in));

	const auto result = ::WSARecvFrom(myListener
		, &recv_wbuffer, 1U
		, &recv_bytes
		, &recv_flags
		, reinterpret_cast<SOCKADDR*>(address), &sockaddr_length
		, nullptr, nullptr);

	if (SOCKET_ERROR == result)
	{
		const auto error_code = WSAGetLastError();

		//if (WSAEWOULDBLOCK == error_code or WSA_IO_PENDING == error_code)
		{
		//	return std::expected<void, int>{};
		}
		//else
		{
			return std::unexpected{ error_code };
		}
	}

	std::println("[Receive] {} bytes\n"
		"Address family: {}\n"
		"Address: {}\n"
		"Address port: {}\n"
		"Received data: {}"
		
		, recv_bytes
		, address->sin_family
		, address->sin_addr.S_un.S_addr
		, ::ntohs(address->sin_port)
		, buffer);

	EndReceive();

	std::println("Echoing the data.");

	::WSABUF send_wbuffer
	{
		.len = recv_bytes,
		.buf = buffer
	};
	::DWORD sent_bytes{};

	const auto sent = ::WSASendTo(myListener
		, &send_wbuffer, 1, &sent_bytes
		, 0
		, reinterpret_cast<const SOCKADDR*>(address), sockaddr_length
		, nullptr, nullptr);

	if (SOCKET_ERROR == sent)
	{
		const auto error_code = WSAGetLastError();

		std::println(" Error when sending echo data. Error code is {}.", error_code);

		return std::unexpected{ error_code };
	}
	else
	{
		std::println("Echo data are sent.");
	}

	// continue
	const auto recv = BeginReceive();

	if (not recv)
	{
		const auto error_code = recv.error();

		std::println("A receive error occured, error code: {}", error_code);

		return std::unexpected{ error_code };
	}

	return std::expected<void, int>{};
}

void
Framework::EndReceive()
{}
