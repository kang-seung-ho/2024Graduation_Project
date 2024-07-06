module;
#include <cassert>

module Iconer.Net.TcpReceiver;
import Iconer.Net.Socket;
import <memory>;
import <span>;

iconer::net::TcpReceiver::~TcpReceiver()
noexcept
{
	delete mySocket;
}

iconer::util::Expected<int, iconer::net::ErrorCode>
iconer::net::TcpReceiver::Receive()
{
	auto io = mySocket->Receive(GetCurrentReceiveBuffer());

	if (io)
	{
		recvBytes.fetch_add(static_cast<std::uint32_t>(io.value()), std::memory_order_acq_rel);
	}

	return std::move(io);
}

iconer::net::IoResult
iconer::net::TcpReceiver::BeginOptainMemory(iconer::net::IoContext& context)
{
	return mySocket->OptainReceiveMemory(context, recvBuffer.data(), maxRecvSize);
}

void
iconer::net::TcpReceiver::EndOptainMemory(iconer::net::IoContext& context)
noexcept
{
	context.ClearIoStatus();
	recvBytes.store(0, std::memory_order_release);
}

iconer::net::IoResult
iconer::net::TcpReceiver::BeginReceive(iconer::net::IoContext& context)
{
	return mySocket->Receive(context, GetCurrentReceiveBuffer());
}

bool
iconer::net::TcpReceiver::EndReceive(iconer::net::IoContext& context, const std::uint32_t bytes)
noexcept
{
	context.ClearIoStatus();

	return 0 != recvBytes.fetch_add(bytes, std::memory_order_acq_rel) + bytes;
}

iconer::net::IoResult
iconer::net::TcpReceiver::BeginReceive(iconer::net::IoContext* context)
{
	return BeginReceive(*context);
}

bool
iconer::net::TcpReceiver::EndReceive(iconer::net::IoContext* context, const std::uint32_t bytes)
noexcept
{
	return EndReceive(*context, bytes);
}

iconer::net::IoResult
iconer::net::TcpReceiver::BeginClose(iconer::net::IoContext& context)
{
	return mySocket->AsyncClose(context);
}

iconer::net::IoResult
iconer::net::TcpReceiver::BeginClose(iconer::net::IoContext* context)
{
	return mySocket->AsyncClose(context);
}

void
iconer::net::TcpReceiver::EndClose()
{
	std::memset(recvBuffer.data(), 0, maxRecvSize);
	recvBytes.store(0, std::memory_order_release);
}

std::unique_ptr<std::byte[]>
iconer::net::TcpReceiver::AcquireReceivedData(size_t size)
{
	assert(size <= maxRecvSize);

	const auto ptr = recvBuffer.data();

	std::unique_ptr<std::byte[]> result = std::make_unique<std::byte[]>(size);
	std::memcpy(result.get(), ptr, size);

	std::memcpy(ptr, ptr + size, maxRecvSize - size);

	recvBytes -= static_cast<std::uint32_t>(size);

	return std::move(result);
}

std::span<std::byte>
iconer::net::TcpReceiver::GetReceiveBuffer()
noexcept
{
	return std::span<std::byte>{ recvBuffer.data(), recvBytes.load(std::memory_order_acquire) };
}

std::span<const std::byte>
iconer::net::TcpReceiver::GetReceiveBuffer()
const noexcept
{
	return std::span<const std::byte>{ recvBuffer.data(), recvBytes.load(std::memory_order_acquire) };
}

std::span<std::byte>
iconer::net::TcpReceiver::GetCurrentReceiveBuffer()
noexcept
{
	const auto bytes = recvBytes.load(std::memory_order_acquire);
	return std::span<std::byte>{ recvBuffer.data() + bytes, maxRecvSize - bytes };
}
