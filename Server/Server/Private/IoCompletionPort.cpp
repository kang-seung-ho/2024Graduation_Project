module;
#include <WinSock2.h>
#include <memory>
#include <optional>
#include <thread>

module Iconer.Net.IoCompletionPort;
import Iconer.Utility.Annihilator;

using namespace iconer;

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
	auto ptr = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, std::move(concurrency_hint));

	if (nullptr == ptr)
	{
		return std::unexpected{ AcquireNetworkError() };
	}

	return IoCompletionPort{ ptr };
}

net::IoCompletionPort::IoCompletionPort(void* handle)
noexcept
	: Handler(std::move(handle))
{
}

net::Socket::ActionResult
net::IoCompletionPort::Register(net::Socket& socket, std::uintptr_t id)
noexcept
{
#if _DEBUG
	auto& sk = socket.GetHandle();
	const HANDLE target = reinterpret_cast<::HANDLE>(sk);

	auto& handle = GetHandle();
	auto port = ::CreateIoCompletionPort(target, const_cast<void*>(handle), id, 0);
#else
	auto port = ::CreateIoCompletionPort(reinterpret_cast<::HANDLE>(socket.GetHandle()), GetHandle(), id, 0);
#endif

	if (nullptr == port)
	{
		return AcquireNetworkError();
	}

	return std::nullopt;
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
		error_code = std::move(result).value();
		return false;
	}
}

bool
net::IoCompletionPort::Destroy()
noexcept
{
	if (auto& handle = GetHandle(); nullptr != handle)
	{
		const bool result = (1 == CloseHandle(handle));
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
net::IoCompletionPort::Schedule(net::IoContext& context, std::uintptr_t id, const unsigned long& infobytes)
noexcept
{
	return 0 != ::PostQueuedCompletionStatus(GetHandle()
		, infobytes
		, std::move(id)
		, static_cast<::WSAOVERLAPPED*>(std::addressof(context)));
}

bool
net::IoCompletionPort::Schedule(net::IoContext* context, std::uintptr_t id, const unsigned long& infobytes)
noexcept
{
	return 0 != ::PostQueuedCompletionStatus(GetHandle()
		, infobytes
		, std::move(id)
		, static_cast<::WSAOVERLAPPED*>(context));
}

bool
net::IoCompletionPort::Schedule(net::IoContext& context, std::uintptr_t id, unsigned long&& infobytes)
noexcept
{
	return 0 != ::PostQueuedCompletionStatus(GetHandle()
		, std::move(infobytes)
		, std::move(id)
		, static_cast<::WSAOVERLAPPED*>(std::addressof(context)));
}

bool
net::IoCompletionPort::Schedule(net::IoContext* const context, std::uintptr_t id, unsigned long&& infobytes)
noexcept
{
	return 0 != ::PostQueuedCompletionStatus(GetHandle()
		, std::move(infobytes)
		, std::move(id)
		, static_cast<::WSAOVERLAPPED*>(context));
}

bool
net::IoCompletionPort::Schedule(volatile net::IoContext& context, std::uintptr_t id, const unsigned long& infobytes)
noexcept
{
	return 0 != ::PostQueuedCompletionStatus(GetHandle()
		, infobytes
		, std::move(id)
		, const_cast<::WSAOVERLAPPED*>(static_cast<volatile ::WSAOVERLAPPED*>(std::addressof(context))));
}

bool
net::IoCompletionPort::Schedule(volatile net::IoContext* context, std::uintptr_t id, const unsigned long& infobytes)
noexcept
{
	return 0 != ::PostQueuedCompletionStatus(GetHandle()
		, infobytes
		, std::move(id)
		, const_cast<::WSAOVERLAPPED*>(static_cast<volatile ::WSAOVERLAPPED*>(context)));
}

bool
net::IoCompletionPort::Schedule(volatile net::IoContext& context, std::uintptr_t id, unsigned long&& infobytes)
noexcept
{
	return 0 != ::PostQueuedCompletionStatus(GetHandle()
		, std::move(infobytes)
		, std::move(id)
		, const_cast<::WSAOVERLAPPED*>(static_cast<volatile ::WSAOVERLAPPED*>(std::addressof(context))));
}

bool
net::IoCompletionPort::Schedule(volatile net::IoContext* const context, std::uintptr_t id, unsigned long&& infobytes)
noexcept
{
	return 0 != ::PostQueuedCompletionStatus(GetHandle()
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
		result = ::GetQueuedCompletionStatus(GetHandle()
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

net::Socket::AsyncResult
iconer::net::IoCompletionPort::WaitForMultipleIoResults(std::span<IoEvent> dest, const unsigned long& max_count)
noexcept
{
	std::allocator<::OVERLAPPED_ENTRY> entry_alloc{};
	auto overlapped_entries = entry_alloc.allocate(max_count);

	iconer::util::Annihilator<std::allocator<::OVERLAPPED_ENTRY>> annihilator
	{
		.ptrHandle = overlapped_entries,
		.ptrSize = max_count
	};

	unsigned long removes = 0;
	if (1 == ::GetQueuedCompletionStatusEx(GetHandle()
		, overlapped_entries, max_count
		, std::addressof(removes), INFINITE, FALSE))
	{
		auto it = dest.begin();

		for (unsigned long i = 0; i < removes and it != dest.end(); ++i, ++it)
		{
			auto&& event = *it;
			auto&& entry = *(overlapped_entries + i);

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

	return removes;
}

bool
iconer::net::IoCompletionPort::IsAvailable()
const noexcept
{
	return nullptr != myHandle;
}
