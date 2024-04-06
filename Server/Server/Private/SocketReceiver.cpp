module;
#include <WinSock2.h>

module Iconer.Net.Socket;
import <cstddef>;
import <utility>;
import <coroutine>;

using namespace iconer;

net::Socket::IoResult
net::Socket::Receive(std::span<std::byte> memory)
const noexcept
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

net::Socket::IoResult
net::Socket::Receive(std::span<std::byte> memory, size_t size)
const noexcept
{
	if (int bytes = ::recv(myHandle
		, reinterpret_cast<char*>(memory.data()), static_cast<int>(std::min(memory.size_bytes(), size))
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
net::Socket::Receive(std::byte* const& memory, size_t size)
const noexcept
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
net::Socket::Receive(std::span<std::byte> memory, net::ErrorCode& error_code)
const noexcept
{
	return Receive(memory).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

bool
net::Socket::Receive(std::span<std::byte> memory, size_t size, net::ErrorCode& error_code)
const noexcept
{
	return Receive(memory, std::move(size)).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

bool
net::Socket::Receive(std::byte* const& memory, size_t size, net::ErrorCode& error_code)
const noexcept
{
	return Receive(memory, std::move(size)).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

net::Socket::AsyncResult
net::Socket::Receive(net::IoContext& context, std::span<std::byte> memory)
const noexcept
{
	::WSABUF buffer
	{
		.len = static_cast<::ULONG>(memory.size_bytes()),
		.buf = reinterpret_cast<char*>(const_cast<std::byte*>(memory.data())),
	};

	::DWORD bytes = 0;
	::DWORD flags = 0;
	if (0 == ::WSARecv(myHandle
		, std::addressof(buffer), 1
		, std::addressof(bytes)
		, std::addressof(flags)
		, static_cast<::LPWSAOVERLAPPED>(std::addressof(context))
		, nullptr))
	{
		return bytes;
	}
	else
	{
		if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation)
		{
			return std::unexpected(std::move(error));
		}
		else
		{
			return 0UL;
		}
	}
}

net::Socket::AsyncResult
net::Socket::Receive(net::IoContext& context, std::span<std::byte> memory, size_t size) const noexcept
{
	::WSABUF buffer
	{
		.len = static_cast<::ULONG>(std::min(memory.size_bytes(), size)),
		.buf = reinterpret_cast<char*>(const_cast<std::byte*>(memory.data())),
	};

	::DWORD bytes = 0;
	::DWORD flags = 0;
	if (0 == ::WSARecv(myHandle
		, std::addressof(buffer), 1
		, std::addressof(bytes)
		, std::addressof(flags)
		, static_cast<::LPWSAOVERLAPPED>(std::addressof(context))
		, nullptr))
	{
		return bytes;
	}
	else
	{
		if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation)
		{
			return std::unexpected(std::move(error));
		}
		else
		{
			return 0UL;
		}
	}
}

net::Socket::AsyncResult
net::Socket::Receive(net::IoContext& context, std::byte* const& memory, size_t size)
const noexcept
{
	::WSABUF buffer
	{
		.len = static_cast<::ULONG>(size),
		.buf = reinterpret_cast<char*>(const_cast<std::byte*>(memory)),
	};

	::DWORD bytes = 0;
	::DWORD flags = 0;
	if (0 == ::WSARecv(myHandle
		, std::addressof(buffer), 1
		, std::addressof(bytes)
		, std::addressof(flags)
		, static_cast<::LPWSAOVERLAPPED>(std::addressof(context))
		, nullptr))
	{
		return bytes;
	}
	else
	{
		if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation)
		{
			return std::unexpected(std::move(error));
		}
		else
		{
			return 0UL;
		}
	}
}

bool
net::Socket::Receive(net::IoContext& context, std::span<std::byte> memory, net::ErrorCode& error_code)
const noexcept
{
	return Receive(context, memory).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

bool
net::Socket::Receive(net::IoContext& context, std::span<std::byte> memory, size_t size, net::ErrorCode& error_code)
const noexcept
{
	return Receive(context, memory, std::move(size)).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

bool
net::Socket::Receive(net::IoContext& context, std::byte* const& memory, size_t size, net::ErrorCode& error_code)
const noexcept
{
	return Receive(context, memory, std::move(size)).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

struct AsyncAwaiter
{
	static constexpr bool await_ready() noexcept { return true; }
	static constexpr void await_suspend(std::coroutine_handle<void>) noexcept
	{
	}

	[[nodiscard]]
	bool await_resume() const noexcept
	{
		return 1 == ::WSAGetOverlappedResult(addr_socket->GetHandle()
			, addr_context
			, addr_transferred_byte
			, TRUE
			, addr_flags);
	}

	const net::Socket* addr_socket;
	::LPWSAOVERLAPPED addr_context;
	::DWORD* addr_transferred_byte;
	::DWORD* addr_flags;
};

net::Socket::IoTask
net::Socket::MakeReceiveTask(net::IoContext& context, std::span<std::byte> memory)
const noexcept
{
	::WSABUF buffer
	{
		.len = static_cast<::ULONG>(memory.size_bytes()),
		.buf = reinterpret_cast<char*>(memory.data()),
	};

	::DWORD transferred_bytes = 0;
	::DWORD flags = 0;
	if (0 == ::WSARecv(myHandle
		, std::addressof(buffer), 1
		, std::addressof(transferred_bytes)
		, std::addressof(flags)
		, static_cast<::LPWSAOVERLAPPED>(std::addressof(context))
		, nullptr))
	{
		co_return transferred_bytes;
	}

	if (co_await AsyncAwaiter
		{
			.addr_socket = this,
			.addr_context = static_cast<::LPWSAOVERLAPPED>(std::addressof(context)),
			.addr_transferred_byte = std::addressof(transferred_bytes),
			.addr_flags = std::addressof(flags),
		})
	{
		co_return transferred_bytes;
	}
	else
	{
		co_return std::unexpected{ AcquireNetworkError() };
	}
}

net::Socket::IoTask
net::Socket::MakeReceiveTask(net::IoContext& context, std::span<std::byte> memory, size_t size)
const noexcept
{
	::WSABUF buffer
	{
		.len = static_cast<::ULONG>(memory.size_bytes()),
		.buf = reinterpret_cast<char*>(size),
	};

	::DWORD transferred_bytes = 0;
	::DWORD flags = 0;
	if (0 == ::WSARecv(myHandle
		, std::addressof(buffer), 1
		, std::addressof(transferred_bytes)
		, std::addressof(flags)
		, static_cast<::LPWSAOVERLAPPED>(std::addressof(context))
		, nullptr))
	{
		co_return transferred_bytes;
	}

	if (co_await AsyncAwaiter
		{
			.addr_socket = this,
			.addr_context = static_cast<::LPWSAOVERLAPPED>(std::addressof(context)),
			.addr_transferred_byte = std::addressof(transferred_bytes),
			.addr_flags = std::addressof(flags),
		})
	{
		co_return transferred_bytes;
	}
	else
	{
		co_return std::unexpected{ AcquireNetworkError() };
	}
}

net::Socket::IoTask
net::Socket::MakeReceiveTask(net::IoContext& context, std::byte* const& memory, size_t size)
const noexcept
{
	::WSABUF buffer
	{
		.len = static_cast<::ULONG>(size),
		.buf = reinterpret_cast<char*>(memory),
	};

	::DWORD transferred_bytes = 0;
	::DWORD flags = 0;
	if (0 == ::WSARecv(myHandle
		, std::addressof(buffer), 1
		, std::addressof(transferred_bytes)
		, std::addressof(flags)
		, static_cast<::LPWSAOVERLAPPED>(std::addressof(context))
		, nullptr))
	{
		co_return transferred_bytes;
	}

	if (co_await AsyncAwaiter
		{
			.addr_socket = this,
			.addr_context = static_cast<::LPWSAOVERLAPPED>(std::addressof(context)),
			.addr_transferred_byte = std::addressof(transferred_bytes),
			.addr_flags = std::addressof(flags),
		})
	{
		co_return transferred_bytes;
	}
	else
	{
		co_return std::unexpected{ AcquireNetworkError() };
	}
}

net::Socket::IoTask
net::Socket::MakeReceiveTask(const std::shared_ptr<net::IoContext>& context, std::span<std::byte> memory)
const noexcept
{
	return MakeReceiveTask(*context, std::move(memory));
}

net::Socket::IoTask
net::Socket::MakeReceiveTask(const std::shared_ptr<net::IoContext>& context, std::span<std::byte> memory, size_t size)
const noexcept
{
	return MakeReceiveTask(*context, std::move(memory), std::move(size));
}

net::Socket::IoTask
net::Socket::MakeReceiveTask(const std::shared_ptr<net::IoContext>& context, std::byte* const& memory, size_t size)
const noexcept
{
	return MakeReceiveTask(*context, memory, std::move(size));
}

net::Socket::IoTask
net::Socket::AsyncRecv(net::IoContext& context, std::span<std::byte> memory)
const noexcept
{
	auto task = MakeReceiveTask(context, memory);
	task.StartAsync();

	return std::move(task);
}

net::Socket::IoTask
net::Socket::AsyncRecv(net::IoContext& context, std::span<std::byte> memory, size_t size)
const noexcept
{
	auto task = MakeReceiveTask(context, memory, std::move(size));
	task.StartAsync();

	return std::move(task);
}

net::Socket::IoTask
net::Socket::AsyncRecv(net::IoContext& context, std::byte* const& memory, size_t size)
const noexcept
{
	auto task = MakeReceiveTask(context, memory, std::move(size));
	task.StartAsync();

	return std::move(task);
}
