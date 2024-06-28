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
#undef WSANO_DATA

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

iconer::net::IoResult
iconer::net::Socket::Receive(IoContext& context, std::span<std::byte> memory)
const
{
	if (memory.size_bytes() == 0 or memory.data() == nullptr)
	{
		return std::unexpected{ ErrorCode::WSANO_DATA };
	}
	else
	{
		::WSABUF buffer
		{
			.len = static_cast<::ULONG>(memory.size_bytes()),
			.buf = reinterpret_cast<char*>(const_cast<std::byte*>(memory.data())),
		};

		::DWORD bytes = 0;
		::DWORD flags = 0;

		if (0 != ::WSARecv(myHandle
			, std::addressof(buffer), 1
			, std::addressof(bytes)
			, std::addressof(flags)
			, std::addressof(context)
			, nullptr))
		{
			if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation)
			{
				return std::unexpected{ std::move(error) };
			}
		}
	}

	return {};
}

iconer::net::IoResult
iconer::net::Socket::Receive(IoContext& context, std::span<std::byte> memory, size_t size)
const
{
	if (memory.size_bytes() == 0 or memory.data() == nullptr or size == 0)
	{
		return std::unexpected{ ErrorCode::WSANO_DATA };
	}
	else
	{
		::WSABUF buffer
		{
			.len = static_cast<::ULONG>(size),
			.buf = reinterpret_cast<char*>(const_cast<std::byte*>(memory.data())),
		};

		::DWORD bytes = 0;
		::DWORD flags = 0;

		if (0 != ::WSARecv(myHandle
			, std::addressof(buffer), 1
			, std::addressof(bytes)
			, std::addressof(flags)
			, std::addressof(context)
			, nullptr))
		{
			if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation)
			{
				return std::unexpected{ std::move(error) };
			}
		}
	}

	return {};
}

iconer::net::IoResult
iconer::net::Socket::Receive(IoContext& context, std::byte* memory, size_t size)
const
{
	if (memory == nullptr or size == 0)
	{
		return std::unexpected{ ErrorCode::WSANO_DATA };
	}
	else
	{
		::WSABUF buffer
		{
			.len = static_cast<::ULONG>(size),
			.buf = reinterpret_cast<char*>(memory),
		};

		::DWORD bytes = 0;
		::DWORD flags = 0;

		if (0 != ::WSARecv(myHandle
			, std::addressof(buffer), 1
			, std::addressof(bytes)
			, std::addressof(flags)
			, std::addressof(context)
			, nullptr))
		{
			if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation)
			{
				return std::unexpected{ std::move(error) };
			}
		}
	}

	return {};
}

bool
iconer::net::Socket::Receive(IoContext& context, std::span<std::byte> memory, ErrorCode& outpin)
const
{
	return Receive(context, memory).transform_error(ErrorTransfer{ outpin }).has_value();
}

bool
iconer::net::Socket::Receive(IoContext& context, std::span<std::byte> memory, size_t size, ErrorCode& outpin)
const
{
	return Receive(context, memory, size).transform_error(ErrorTransfer{ outpin }).has_value();
}

bool
iconer::net::Socket::Receive(IoContext& context, std::byte* memory, size_t size, ErrorCode& outpin)
const
{
	return Receive(context, memory, size).transform_error(ErrorTransfer{ outpin }).has_value();
}
