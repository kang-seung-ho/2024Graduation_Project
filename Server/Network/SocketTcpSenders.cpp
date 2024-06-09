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
iconer::net::Socket::Send(std::span<const std::byte> memory)
const
{
	if (memory.size_bytes() == 0)
	{
		return std::unexpected{ ErrorCode::WSANO_DATA };
	}
	else if (const int bytes = ::send(myHandle
		, reinterpret_cast<const char*>(memory.data()), static_cast<int>(memory.size_bytes())
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
iconer::net::Socket::Send(std::span<const std::byte> memory, size_t size)
const
{
	if (memory.size_bytes() == 0 or size == 0 or memory.data() == nullptr)
	{
		return std::unexpected{ ErrorCode::WSANO_DATA };
	}
	else if (const int bytes = ::send(myHandle
		, reinterpret_cast<const char*>(memory.data()), static_cast<int>(size)
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
iconer::net::Socket::Send(const std::byte* const& memory, size_t size)
const
{
	if (size == 0 or memory == nullptr)
	{
		return std::unexpected{ ErrorCode::WSANO_DATA };
	}
	else if (const int bytes = ::send(myHandle
		, reinterpret_cast<const char*>(memory), static_cast<int>(size)
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
iconer::net::Socket::Send(std::span<const std::byte> memory, iconer::net::ErrorCode& outpin)
const
{
	return Send(memory).transform_error(ErrorTransfer{ outpin }).has_value();
}

bool
iconer::net::Socket::Send(std::span<const std::byte> memory, size_t size, iconer::net::ErrorCode& outpin)
const
{
	return Send(memory, size).transform_error(ErrorTransfer{ outpin }).has_value();
}

bool
iconer::net::Socket::Send(const std::byte* const& memory, size_t size, iconer::net::ErrorCode& outpin)
const
{
	return Send(memory, size).transform_error(ErrorTransfer{ outpin }).has_value();
}

iconer::net::Socket::IoResult
iconer::net::Socket::Send(iconer::net::IoContext& context, std::span<const std::byte> memory)
const
{
	::WSABUF buffer
	{
		.len = static_cast<::ULONG>(memory.size_bytes()),
		.buf = reinterpret_cast<char*>(const_cast<std::byte*>(memory.data())),
	};

	return RawSendEx(myHandle, buffer, std::addressof(context), nullptr);
}

iconer::net::Socket::IoResult
iconer::net::Socket::Send(iconer::net::IoContext& context, std::span<const std::byte> memory, size_t size)
const
{
	::WSABUF buffer
	{
		.len = static_cast<::ULONG>(size),
		.buf = reinterpret_cast<char*>(const_cast<std::byte*>(memory.data())),
	};

	return RawSendEx(myHandle, buffer, std::addressof(context), nullptr);
}

iconer::net::Socket::IoResult
iconer::net::Socket::Send(iconer::net::IoContext& context, const std::byte* const& memory, size_t size)
const
{
	::WSABUF buffer
	{
		.len = static_cast<::ULONG>(size),
		.buf = reinterpret_cast<char*>(const_cast<std::byte*>(memory)),
	};

	return RawSendEx(myHandle, buffer, std::addressof(context), nullptr);
}

bool
iconer::net::Socket::Send(iconer::net::IoContext& context, std::span<const std::byte> memory, iconer::net::ErrorCode& outpin)
const
{
	return Send(context, memory).transform_error(ErrorTransfer{ outpin }).has_value();
}

bool
iconer::net::Socket::Send(iconer::net::IoContext& context, std::span<const std::byte> memory, size_t size, iconer::net::ErrorCode& outpin)
const
{
	return Send(context, memory, size).transform_error(ErrorTransfer{ outpin }).has_value();
}

bool
iconer::net::Socket::Send(iconer::net::IoContext& context, const std::byte* const& memory, size_t size, iconer::net::ErrorCode& outpin)
const
{
	return Send(context, memory, size).transform_error(ErrorTransfer{ outpin }).has_value();
}

iconer::net::Socket::IoResult
RawSendEx(const std::uintptr_t& socket
	, ::WSABUF& buffer
	, void* context
	, ::LPWSAOVERLAPPED_COMPLETION_ROUTINE routine)
	noexcept
{
	if (::DWORD bytes = 0; 0 == ::WSASend(socket
		, std::addressof(buffer), 1
		, std::addressof(bytes)
		, 0
		, static_cast<::LPWSAOVERLAPPED>(context)
		, routine))
	{
		return {};
	}
	else
	{
		if (auto error = iconer::net::AcquireNetworkError(); error != iconer::net::ErrorCode::PendedIoOperation)
		{
			return std::unexpected{ std::move(error) };
		}
		else
		{
			return {};
		}
	}
}
