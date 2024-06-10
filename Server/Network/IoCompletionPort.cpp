module;
#include <WinSock2.h>

module Iconer.Net.IoCompletionPort;
import Iconer.Net.Socket;
import <memory>;
import <optional>;
import <vector>;
import <thread>;

using namespace iconer;

net::IoCompletionPort::IoCompletionPort(void* handle)
noexcept
	: myHandle(handle)
{}

net::IoCompletionPort::FactoryResult
net::IoCompletionPort::Create()
noexcept
{
	return Create(std::thread::hardware_concurrency());
}

net::IoCompletionPort::FactoryResult
net::IoCompletionPort::Create(std::uint32_t concurrency_hint)
noexcept
{
	const auto port = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, std::move(concurrency_hint));

	if (nullptr == port)
	{
		return std::unexpected{ AcquireNetworkError() };
	}

	return IoCompletionPort{ port };
}

std::expected<void, iconer::net::ErrorCode>
net::IoCompletionPort::Register(net::Socket& socket, std::uintptr_t id)
noexcept
{
#if _DEBUG
	const HANDLE target = reinterpret_cast<::HANDLE>(socket.GetNativeHandle());

	const auto port = ::CreateIoCompletionPort(target, myHandle, id, 0);
#else
	auto port = ::CreateIoCompletionPort(reinterpret_cast<::HANDLE>(socket.myHandle), myHandle, id, 0);
#endif

	if (nullptr == port)
	{
		return std::unexpected{ AcquireNetworkError() };
	}

	return {};
}

bool
net::IoCompletionPort::TryRegister(net::Socket& socket, std::uintptr_t id, net::ErrorCode& error_code)
noexcept
{
	if (auto result = Register(socket, id); result)
	{
		return true;
	}
	else
	{
		error_code = std::move(result).error();

		return false;
	}
}

bool
net::IoCompletionPort::Destroy()
noexcept
{
	if (auto& handle = myHandle; nullptr != handle)
	{
		const bool result = (0 != CloseHandle(handle));
		myHandle = nullptr;

		return result;
	}
	else
	{
		return false;
	}
}

bool
net::IoCompletionPort::Destroy(net::ErrorCode& error_code)
noexcept
{
	if (auto result = Destroy(); result)
	{
		return true;
	}
	else
	{
		error_code = AcquireNetworkError();

		return false;
	}
}

bool
net::IoCompletionPort::Schedule(net::IoContext& context, std::uintptr_t id, unsigned long infobytes)
noexcept
{
	return 0 != ::PostQueuedCompletionStatus(myHandle
		, std::move(infobytes)
		, std::move(id)
		, static_cast<::WSAOVERLAPPED*>(std::addressof(context)));
}

bool
net::IoCompletionPort::Schedule(net::IoContext* const context, std::uintptr_t id, unsigned long infobytes)
noexcept
{
	return 0 != ::PostQueuedCompletionStatus(myHandle
		, std::move(infobytes)
		, std::move(id)
		, static_cast<::WSAOVERLAPPED*>(context));
}

bool
net::IoCompletionPort::Schedule(volatile net::IoContext& context, std::uintptr_t id, unsigned long infobytes)
noexcept
{
	return 0 != ::PostQueuedCompletionStatus(myHandle
		, infobytes
		, std::move(id)
		, const_cast<::WSAOVERLAPPED*>(static_cast<volatile ::WSAOVERLAPPED*>(&context)));
}

bool
net::IoCompletionPort::Schedule(volatile net::IoContext* const context, std::uintptr_t id, unsigned long infobytes)
noexcept
{
	return 0 != ::PostQueuedCompletionStatus(myHandle
		, std::move(infobytes)
		, std::move(id)
		, const_cast<::WSAOVERLAPPED*>(static_cast<volatile ::WSAOVERLAPPED*>(context)));
}

net::IoEvent
net::IoCompletionPort::WaitForIoResult()
noexcept
{
	net::IoEvent ev_handle{};

	::LPOVERLAPPED overlapped{};
	::BOOL result = 0;
	try
	{
		result = ::GetQueuedCompletionStatus(myHandle
			, std::addressof(ev_handle.ioBytes)
			, std::addressof(ev_handle.eventId)
			, std::addressof(overlapped)
			, INFINITE);

		ev_handle.ioContext = static_cast<net::IoContext*>(overlapped);
		ev_handle.isSucceed = (1 == result);
	}
	catch (...)
	{
		ev_handle.ioContext = nullptr;
		ev_handle.isSucceed = false;
	}

	return ev_handle;
}

std::expected<void, iconer::net::ErrorCode>
iconer::net::IoCompletionPort::WaitForMultipleIoResults(std::span<IoEvent> dest, unsigned long max_count)
{
	std::vector<::OVERLAPPED_ENTRY> overlapped_entries{};

	overlapped_entries.resize(max_count);

	unsigned long removes = 0;
	if (1 == ::GetQueuedCompletionStatusEx(myHandle
		, overlapped_entries.data(), max_count
		, std::addressof(removes), INFINITE, FALSE))
	{
		auto it = dest.begin();

		for (unsigned long i = 0; i < removes and it != dest.end(); ++i, ++it)
		{
			auto&& event = *it;
			auto&& entry = overlapped_entries[i];

			event.isSucceed = true;
			event.eventId = entry.lpCompletionKey;
			event.ioBytes = entry.dwNumberOfBytesTransferred;
			event.ioContext = static_cast<iconer::net::IoContext*>(entry.lpOverlapped);
		}
	}
	else
	{
		return std::unexpected{ AcquireNetworkError() };
	}

	return {};
}
