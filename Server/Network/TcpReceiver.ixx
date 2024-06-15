export module Iconer.Net.TcpReceiver;
import Iconer.Net.ErrorCode;
import Iconer.Net.Socket;
import Iconer.Net.IoContext;
import Iconer.Net.SocketWrapper;
import <cstddef>;
import <cstdint>;
import <expected>;
import <vector>;
import <span>;

export namespace iconer::net
{
	class [[nodiscard]] TcpReceiver : public iconer::net::SocketWrapper
	{
	public:
		using super = iconer::net::SocketWrapper;
		using this_class = TcpReceiver;

		std::vector<std::byte> recvBuffer;
		std::uint32_t maxRecvSize;
		std::uint32_t recvBytes;

		constexpr ~TcpReceiver() noexcept = default;

		constexpr TcpReceiver(iconer::net::Socket&& socket, std::uint32_t buffer_size)
			: super(static_cast<iconer::net::Socket&&>(socket))
			, recvBuffer(buffer_size, std::byte{}), maxRecvSize(buffer_size)
			, recvBytes()
		{}

		[[nodiscard]]
		std::expected<int, iconer::net::ErrorCode> Receive()
		{
			auto span = GetReceiveBuffer();
			auto io = mySocket.Receive(span);

			if (io)
			{
				recvBytes += static_cast<std::uint32_t>(io.value());
			}

			return std::move(io);
		}

		std::expected<void, iconer::net::ErrorCode> BeginReceive(iconer::net::IoContext& context)
		{
			auto span = GetReceiveBuffer();
			return mySocket.Receive(context, span);
		}

		bool EndReceive(iconer::net::IoContext& context, const std::uint32_t bytes)
		{
			context.ClearIoStatus();

			recvBytes += bytes;

			return 0 != bytes;
		}

		std::expected<void, iconer::net::ErrorCode> BeginReceive(iconer::net::IoContext* context)
		{
			auto span = GetCurrentReceiveBuffer();
			return mySocket.Receive(*context, span);
		}

		bool EndReceive(iconer::net::IoContext* context, const std::uint32_t bytes)
		{
			return EndReceive(*context, bytes);
		}

		[[nodiscard]]
		std::span<std::byte> GetReceiveBuffer() noexcept
		{
			return std::span<std::byte>{ recvBuffer.data(), maxRecvSize };
		}

		[[nodiscard]]
		std::span<const std::byte> GetReceiveBuffer() const noexcept
		{
			return std::span<const std::byte>{ recvBuffer.data(), maxRecvSize };
		}

		[[nodiscard]]
		std::span<std::byte> GetCurrentReceiveBuffer() noexcept
		{
			return std::span<std::byte>{ recvBuffer.data() + recvBytes, maxRecvSize - recvBytes };
		}

		TcpReceiver(TcpReceiver&&) = default;
		TcpReceiver& operator=(TcpReceiver&&) = default;

	private:
		TcpReceiver(const TcpReceiver&) = delete;
		TcpReceiver& operator=(const TcpReceiver&) = delete;
	};
}
