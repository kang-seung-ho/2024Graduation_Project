module;
#include <WinSock2.h>

module Iconer.Net.Socket;
import <cstddef>;
import <memory>;
import <utility>;
import <coroutine>;
import <atomic>;

using namespace iconer;

inline net::Socket::AsyncResult RawSend(const net::Socket::HandleType& sock, ::WSABUF& buffer) noexcept;
inline net::Socket::AsyncResult RawSendEx(const net::Socket::HandleType& sock, ::WSABUF& buffer, void* context, ::LPWSAOVERLAPPED_COMPLETION_ROUTINE routine) noexcept;

net::Socket::IoResult
net::Socket::Send(std::span<const std::byte> memory)
const noexcept
{
	if (int bytes = ::send(myHandle
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

net::Socket::IoResult
net::Socket::Send(std::span<const std::byte> memory, size_t size) const noexcept
{
	::WSABUF buffer
	{
		.len = static_cast<::ULONG>(std::min(memory.size_bytes(), size)),
		.buf = reinterpret_cast<char*>(const_cast<std::byte*>(memory.data())),
	};

	return RawSend(myHandle, buffer);
}

net::Socket::IoResult
net::Socket::Send(const std::byte* const& memory, size_t size)
const noexcept
{
	::WSABUF buffer
	{
		.len = static_cast<::ULONG>(size),
		.buf = reinterpret_cast<char*>(const_cast<std::byte*>(memory)),
	};

	return RawSend(myHandle, buffer);
}

bool
net::Socket::Send(std::span<const std::byte> memory, net::ErrorCode& error_code)
const noexcept
{
	return Send(memory).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

bool
net::Socket::Send(std::span<const std::byte> memory, size_t size, net::ErrorCode& error_code)
const noexcept
{
	return Send(memory, std::move(size)).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

bool
net::Socket::Send(const std::byte* const& memory, size_t size, net::ErrorCode& error_code)
const noexcept
{
	return Send(memory, std::move(size)).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

net::Socket::AsyncResult
net::Socket::Send(net::IoContext& context, std::span<const std::byte> memory)
const noexcept
{
	::WSABUF buffer
	{
		.len = static_cast<::ULONG>(memory.size_bytes()),
		.buf = reinterpret_cast<char*>(const_cast<std::byte*>(memory.data())),
	};

	return RawSendEx(myHandle, buffer, std::addressof(context), nullptr);
}

net::Socket::AsyncResult
net::Socket::Send(net::IoContext& context, std::span<const std::byte> memory, size_t size)
const noexcept
{
	::WSABUF buffer
	{
		.len = static_cast<::ULONG>(std::min(memory.size_bytes(), size)),
		.buf = reinterpret_cast<char*>(const_cast<std::byte*>(memory.data())),
	};

	return RawSendEx(myHandle, buffer, std::addressof(context), nullptr);
}

net::Socket::AsyncResult
net::Socket::Send(net::IoContext& context, const std::byte* const& memory, size_t size)
const noexcept
{
	::WSABUF buffer
	{
		.len = static_cast<::ULONG>(size),
		.buf = reinterpret_cast<char*>(const_cast<std::byte*>(memory)),
	};

	return RawSendEx(myHandle, buffer, std::addressof(context), nullptr);
}

bool
net::Socket::Send(net::IoContext& context, std::span<const std::byte> memory, net::ErrorCode& error_code)
const noexcept
{
	return Send(context, memory).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

bool
net::Socket::Send(net::IoContext& context, std::span<const std::byte> memory, size_t size, net::ErrorCode& error_code)
const noexcept
{
	return Send(context, memory, std::move(size)).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

bool
net::Socket::Send(net::IoContext& context, const std::byte* const& memory, size_t size, net::ErrorCode& error_code)
const noexcept
{
	return Send(context, memory, std::move(size)).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

net::Socket::IoTask
net::Socket::MakeSendTask(net::IoContext& context, std::span<const std::byte> memory)
const noexcept
{
	if (auto sent = Send(context, memory); sent.has_value())
	{
		co_return std::move(sent);
	}

	static ::DWORD flags = 0;
	::DWORD transferred_bytes = 0;

	::BOOL result = ::WSAGetOverlappedResult(myHandle
		, static_cast<::LPWSAOVERLAPPED>(std::addressof(context))
		, std::addressof(transferred_bytes)
		, TRUE
		, std::addressof(flags));

	if (FALSE == result)
	{
		co_return std::unexpected{ net::AcquireNetworkError() };
	}
	else
	{
		co_return transferred_bytes;
	}
}

net::Socket::IoTask
net::Socket::MakeSendTask(net::IoContext& context, std::span<const std::byte> memory, size_t size)
const noexcept
{
	if (auto sent = Send(context, memory, size); sent.has_value())
	{
		co_return std::move(sent);
	}

	static ::DWORD flags = 0;
	::DWORD transferred_bytes = 0;

	::BOOL result = ::WSAGetOverlappedResult(myHandle
		, static_cast<::LPWSAOVERLAPPED>(std::addressof(context))
		, std::addressof(transferred_bytes)
		, TRUE
		, std::addressof(flags));

	if (FALSE == result)
	{
		co_return std::unexpected{ net::AcquireNetworkError() };
	}
	else
	{
		co_return transferred_bytes;
	}
}

net::Socket::IoTask
net::Socket::MakeSendTask(net::IoContext& context, const std::byte* const& memory, size_t size)
const noexcept
{
	if (auto sent = Send(context, memory, size); sent.has_value())
	{
		co_return std::move(sent);
	}

	static ::DWORD flags = 0;
	::DWORD transferred_bytes = 0;

	::BOOL result = ::WSAGetOverlappedResult(myHandle
		, static_cast<::LPWSAOVERLAPPED>(std::addressof(context))
		, std::addressof(transferred_bytes)
		, TRUE
		, std::addressof(flags));

	if (FALSE == result)
	{
		co_return std::unexpected{ net::AcquireNetworkError() };
	}
	else
	{
		co_return transferred_bytes;
	}
}

net::Socket::IoTask
net::Socket::MakeSendTask(const std::shared_ptr<net::IoContext>& context, std::span<const std::byte> memory)
const noexcept
{
	return MakeSendTask(*context, std::move(memory));
}

net::Socket::IoTask
net::Socket::MakeSendTask(const std::shared_ptr<net::IoContext>& context, std::span<const std::byte> memory, size_t size)
const noexcept
{
	return MakeSendTask(*context, std::move(memory), std::move(size));
}

net::Socket::IoTask
net::Socket::MakeSendTask(const std::shared_ptr<net::IoContext>& context, const std::byte* const& memory, size_t size)
const noexcept
{
	return MakeSendTask(*context, memory, std::move(size));
}

net::Socket::IoTask
net::Socket::AsyncSend(net::IoContext& context, std::span<const std::byte> memory)
const noexcept
{
	auto task = MakeSendTask(context, memory);
	task.StartAsync();

	return std::move(task);
}

net::Socket::IoTask
net::Socket::AsyncSend(net::IoContext& context, std::span<const std::byte> memory, size_t size)
const noexcept
{
	auto task = MakeSendTask(context, memory, size);
	task.StartAsync();

	return std::move(task);
}

net::Socket::IoTask
net::Socket::AsyncSend(net::IoContext& context, const std::byte* const& memory, size_t size)
const noexcept
{
	auto task = MakeSendTask(context, memory, size);
	task.StartAsync();

	return std::move(task);
}

net::Socket::AsyncResult
RawSend(const net::Socket::HandleType& sock
	, ::WSABUF& buffer)
	noexcept
{
	if (::DWORD bytes = 0; 0 == ::WSASend(sock
		, std::addressof(buffer), 1
		, std::addressof(bytes)
		, 0
		, nullptr, nullptr))
	{
		return bytes;
	}
	else
	{
		return std::unexpected{ net::AcquireNetworkError() };
	}
}

net::Socket::AsyncResult
RawSendEx(const net::Socket::HandleType& sock
	, ::WSABUF& buffer
	, void* context
	, ::LPWSAOVERLAPPED_COMPLETION_ROUTINE routine)
	noexcept
{
	if (::DWORD bytes = 0; 0 == ::WSASend(sock
		, std::addressof(buffer), 1
		, std::addressof(bytes)
		, 0
		, static_cast<::LPWSAOVERLAPPED>(context)
		, routine))
	{
		return bytes;
	}
	else
	{
		if (net::ErrorCode error = net::AcquireNetworkError(); error != net::ErrorCode::PendedIoOperation)
		{
			return std::unexpected{ std::move(error) };
		}
		else
		{
			return 0UL;
		}
	}
}
