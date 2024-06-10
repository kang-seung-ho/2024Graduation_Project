module;
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ws2ipdef.h>
#include <MSWSock.h>

#define LIKELY [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Net.Socket;
import Iconer.Net.IpAddress;
import Iconer.Net.EndPoint;
import Iconer.Net.IoContext;

iconer::net::Socket::SyncIoResult
iconer::net::Socket::Receive(std::span<std::byte> memory)
const
{
	if (int bytes = ::recv(myHandle
		, reinterpret_cast<char*>(memory.data()), static_cast<int>(memory.size_bytes())
		, 0); SOCKET_ERROR != bytes)
	{
		return bytes;
	}
	else
	{
		return std::unexpected{ AcquireNetworkError() };
	}
}

iconer::net::Socket::SyncIoResult
iconer::net::Socket::Receive(std::span<std::byte> memory, size_t size)
const
{
	if (int bytes = ::recv(myHandle
		, reinterpret_cast<char*>(memory.data()), static_cast<int>(size)
		, 0); SOCKET_ERROR != bytes)
	{
		return bytes;
	}
	else
	{
		return std::unexpected{ AcquireNetworkError() };
	}
}

iconer::net::Socket::SyncIoResult
iconer::net::Socket::Receive(std::byte* memory, size_t size)
const
{
	if (int bytes = ::recv(myHandle
		, reinterpret_cast<char*>(memory), static_cast<int>(size)
		, 0); SOCKET_ERROR != bytes)
	{
		return bytes;
	}
	else
	{
		return std::unexpected{ AcquireNetworkError() };
	}
}

bool
iconer::net::Socket::Receive(std::span<std::byte> memory, ErrorCode& outpin)
const
{
	return Receive(memory).transform_error(ErrorTransfer{ outpin }).has_value();
}

bool
iconer::net::Socket::Receive(std::span<std::byte> memory, size_t size, ErrorCode& outpin)
const
{
	return Receive(memory, size).transform_error(ErrorTransfer{ outpin }).has_value();
}

bool
iconer::net::Socket::Receive(std::byte* memory, size_t size, ErrorCode& outpin)
const
{
	return Receive(memory, size).transform_error(ErrorTransfer{ outpin }).has_value();
}

iconer::net::Socket::IoResult
iconer::net::Socket::Receive(IoContext& context, std::span<std::byte> memory)
const
{
	return IoResult();
}

iconer::net::Socket::IoResult
iconer::net::Socket::Receive(IoContext& context, std::span<std::byte> memory, size_t size)
const
{
	return IoResult();
}

iconer::net::Socket::IoResult
iconer::net::Socket::Receive(IoContext& context, std::byte* memory, size_t size)
const
{
	return IoResult();
}

bool
iconer::net::Socket::Receive(IoContext& context, std::span<std::byte> memory, ErrorCode& outpin)
const
{
	return false;
}

bool
iconer::net::Socket::Receive(IoContext& context, std::span<std::byte> memory, size_t size, ErrorCode& outpin)
const
{
	return false;
}

bool
iconer::net::Socket::Receive(IoContext& context, std::byte* memory, size_t size, ErrorCode& outpin)
const
{
	return false;
}
