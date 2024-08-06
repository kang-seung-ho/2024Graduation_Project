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
		recvBytes += static_cast<std::uint32_t>(io.value());
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

	recvBytes += bytes;

	return 0 != recvBytes;
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

	const auto last_off = maxRecvSize - size;

	std::memmove(ptr, ptr + size, maxRecvSize - size);
	//std::memset(ptr + size, 0, maxRecvSize - size);

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
iconer::net::TcpReceiver::GetReceiveBuffer(const size_t& size)
noexcept
{
	assert(0 < size);

	return std::span<std::byte>{ recvBuffer.data(), size };
}

std::span<std::byte>
iconer::net::TcpReceiver::GetReceiveBuffer()
noexcept
{
	assert(0 < recvBytes);

	return std::span<std::byte>{ recvBuffer.data(), size_t(recvBytes) };
}

std::span<std::byte>
iconer::net::TcpReceiver::GetCurrentReceiveBuffer()
noexcept
{
	return std::span<std::byte>{ recvBuffer.data() + ptrdiff_t(recvBytes), maxRecvSize - uint32_t(recvBytes) };
}
