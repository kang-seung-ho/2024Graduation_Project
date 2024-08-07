module;
#include <WinSock2.h>

module Iconer.Net.IoCompletionPort;
import Iconer.Net.Socket;
import Iconer.Net.IoContext;
import <memory>;
import <optional>;
import <vector>;
import <thread>;

iconer::net::IoCompletionPort::IoCompletionPort(void* handle)
noexcept
	: myHandle(handle)
{}

iconer::net::IoCompletionPort::FactoryResult
iconer::net::IoCompletionPort::Create()
noexcept
{
	return Create(std::thread::hardware_concurrency());
}

iconer::net::IoCompletionPort::FactoryResult
iconer::net::IoCompletionPort::Create(std::uint32_t concurrency_hint)
noexcept
{
	const auto port = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, std::move(concurrency_hint));

	if (nullptr == port)
	{
		return std::unexpected{ AcquireNetworkError() };
	}

	return IoCompletionPort{ port };
}

iconer::net::IoResult
iconer::net::IoCompletionPort::Register(iconer::net::Socket& socket, std::uintptr_t id)
const noexcept
{
#if _DEBUG
	const HANDLE target = reinterpret_cast<::HANDLE>(socket.GetNativeHandle());

	const auto port = ::CreateIoCompletionPort(target, myHandle, id, 0);
#else
	auto port = ::CreateIoCompletionPort(reinterpret_cast<::HANDLE>(socket.GetNativeHandle()), myHandle, id, 0);
#endif

	if (nullptr == port)
	{
		return std::unexpected{ AcquireNetworkError() };
	}

	return {};
}

bool
iconer::net::IoCompletionPort::TryRegister(iconer::net::Socket& socket, std::uintptr_t id, iconer::net::ErrorCode& error_code)
const noexcept
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
iconer::net::IoCompletionPort::Destroy()
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
iconer::net::IoCompletionPort::Destroy(iconer::net::ErrorCode& error_code)
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
iconer::net::IoCompletionPort::Schedule(iconer::net::IoContext& context, std::uintptr_t id, unsigned long infobytes)
const noexcept
{
	return 0 != ::PostQueuedCompletionStatus(myHandle
		, std::move(infobytes)
		, std::move(id)
		, static_cast<::WSAOVERLAPPED*>(std::addressof(context)));
}

bool
iconer::net::IoCompletionPort::Schedule(iconer::net::IoContext* const context, std::uintptr_t id, unsigned long infobytes)
const noexcept
{
	return 0 != ::PostQueuedCompletionStatus(myHandle
		, std::move(infobytes)
		, std::move(id)
		, static_cast<::WSAOVERLAPPED*>(context));
}

bool
iconer::net::IoCompletionPort::Schedule(volatile iconer::net::IoContext& context, std::uintptr_t id, unsigned long infobytes)
const noexcept
{
	return 0 != ::PostQueuedCompletionStatus(myHandle
		, infobytes
		, std::move(id)
		, const_cast<::WSAOVERLAPPED*>(static_cast<volatile ::WSAOVERLAPPED*>(&context)));
}

bool
iconer::net::IoCompletionPort::Schedule(volatile iconer::net::IoContext* const context, std::uintptr_t id, unsigned long infobytes)
const noexcept
{
	return 0 != ::PostQueuedCompletionStatus(myHandle
		, std::move(infobytes)
		, std::move(id)
		, const_cast<::WSAOVERLAPPED*>(static_cast<volatile ::WSAOVERLAPPED*>(context)));
}

iconer::net::IoEvent
iconer::net::IoCompletionPort::WaitForIoResult()
const noexcept
{
	iconer::net::IoEvent ev_handle{};

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

iconer::net::IoResult
iconer::net::IoCompletionPort::WaitForMultipleIoResults(std::span<IoEvent> dest, unsigned long max_count)
const
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
			auto&& entry = overlapped_entries.at(i);

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
