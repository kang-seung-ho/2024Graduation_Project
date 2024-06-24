export module Iconer.Net.TcpReceiver;
import Iconer.Net.ErrorCode;
import Iconer.Net.Socket;
import Iconer.Net.IoContext;
import Iconer.Net.SocketWrapper;
import <cstddef>;
import <cstdint>;
import <expected>;
import <vector>;

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

		constexpr TcpReceiver(iconer::net::Socket&& socket, std::uint32_t buffer_size)
			: super(static_cast<iconer::net::Socket&&>(socket))
			, recvBuffer(buffer_size, std::byte{}), maxRecvSize(buffer_size)
			, recvBytes()
		{}

		[[nodiscard]] std::expected<int, iconer::net::ErrorCode> Receive();

		std::expected<void, iconer::net::ErrorCode> BeginReceive(iconer::net::IoContext& context);
		bool EndReceive(iconer::net::IoContext& context, const std::uint32_t bytes);

		std::expected<void, iconer::net::ErrorCode> BeginReceive(iconer::net::IoContext* context);
		bool EndReceive(iconer::net::IoContext* context, const std::uint32_t bytes);

		[[nodiscard]] std::span<std::byte> GetReceiveBuffer() noexcept;
		[[nodiscard]] std::span<const std::byte> GetReceiveBuffer() const noexcept;
		[[nodiscard]] std::span<std::byte> GetCurrentReceiveBuffer() noexcept;

		TcpReceiver(TcpReceiver&&) = default;
		TcpReceiver& operator=(TcpReceiver&&) = default;

	private:
		TcpReceiver(const TcpReceiver&) = delete;
		TcpReceiver& operator=(const TcpReceiver&) = delete;
	};
}
