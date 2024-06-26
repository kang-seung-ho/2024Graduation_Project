module Iconer.Net.TcpReceiver;
import <span>;

std::expected<int, iconer::net::ErrorCode>
iconer::net::TcpReceiver::Receive()
{
	auto span = GetReceiveBuffer();
	auto io = mySocket.Receive(span);

	if (io)
	{
		recvBytes += static_cast<std::uint32_t>(io.value());
	}

	return std::move(io);
}

std::expected<void, iconer::net::ErrorCode>
iconer::net::TcpReceiver::BeginReceive(iconer::net::IoContext& context)
{
	auto span = GetReceiveBuffer();
	return mySocket.Receive(context, span);
}

bool
iconer::net::TcpReceiver::EndReceive(iconer::net::IoContext& context, const std::uint32_t bytes)
noexcept
{
	context.ClearIoStatus();

	recvBytes += bytes;

	return 0 != bytes;
}

std::expected<void, iconer::net::ErrorCode>
iconer::net::TcpReceiver::BeginReceive(iconer::net::IoContext* context)
{
	auto span = GetCurrentReceiveBuffer();
	return mySocket.Receive(*context, span);
}

bool
iconer::net::TcpReceiver::EndReceive(iconer::net::IoContext* context, const std::uint32_t bytes)
noexcept
{
	return EndReceive(*context, bytes);
}

std::span<std::byte>
iconer::net::TcpReceiver::GetReceiveBuffer()
noexcept
{
	return std::span<std::byte>{ recvBuffer.data(), maxRecvSize };
}

std::span<const std::byte>
iconer::net::TcpReceiver::GetReceiveBuffer()
const noexcept
{
	return std::span<const std::byte>{ recvBuffer.data(), maxRecvSize };
}

std::span<std::byte>
iconer::net::TcpReceiver::GetCurrentReceiveBuffer()
noexcept
{
	return std::span<std::byte>{ recvBuffer.data() + recvBytes, maxRecvSize - recvBytes };
}
