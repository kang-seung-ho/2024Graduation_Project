module;
#include <cassert>

module Iconer.Net.TcpReceiver;
import Iconer.Net.Socket;
import <memory>;
import <span>;

iconer::util::Expected<int, iconer::net::ErrorCode>
iconer::net::TcpReceiver::Receive()
{
	auto io = GetSocket().Receive(GetCurrentReceiveBuffer());

	if (io)
	{
		recvBytes.fetch_add(static_cast<std::uint32_t>(io.value()), std::memory_order_acq_rel);
	}

	return std::move(io);
}

iconer::net::IoResult
iconer::net::TcpReceiver::BeginOptainMemory(iconer::net::IoContext& context)
{
	return GetSocket().OptainReceiveMemory(context, recvBuffer.data(), maxRecvSize);
}

void
iconer::net::TcpReceiver::EndOptainMemory(iconer::net::IoContext& context)
noexcept
{
	context.ClearIoStatus();
	recvBytes = 0;
}

iconer::net::IoResult
iconer::net::TcpReceiver::BeginReceive(iconer::net::IoContext& context)
{
	return GetSocket().Receive(context, GetCurrentReceiveBuffer());
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
	return GetSocket().AsyncClose(context);
}

iconer::net::IoResult
iconer::net::TcpReceiver::BeginClose(iconer::net::IoContext* context)
{
	return GetSocket().AsyncClose(context);
}

void
iconer::net::TcpReceiver::EndClose()
{
	std::memset(recvBuffer.data(), 0, maxRecvSize);
	recvBytes = 0;
}

void
iconer::net::TcpReceiver::PullReceivedData(const size_t& size)
{
	assert(0 < size);
	assert(size <= maxRecvSize);

	const auto ptr = recvBuffer.data();

	const std::uint32_t sz = static_cast<std::uint32_t>(size);

	const auto last_off = maxRecvSize - size;
	std::memcpy(ptr, ptr + size, maxRecvSize - size);
	//std::memset(ptr + last_off, 0, size);

	recvBytes -= static_cast<std::uint32_t>(size);
}

std::unique_ptr<std::byte[]>
iconer::net::TcpReceiver::AcquireReceivedData(const size_t& size)
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
	const auto recvs = recvBytes.load();
	assert(0 < recvs);

	return std::span<std::byte>{ recvBuffer.data(), static_cast<size_t>(recvs) };
}

std::span<std::byte>
iconer::net::TcpReceiver::GetCurrentReceiveBuffer()
noexcept
{
	const auto recvs = recvBytes.load();
	return std::span<std::byte>{ recvBuffer.data() + recvs, maxRecvSize - static_cast<size_t>(recvs) };
}
