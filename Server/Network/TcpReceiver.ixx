export module Iconer.Net.TcpReceiver;
import Iconer.Net.ErrorCode;
import Iconer.Net.IoResult;
import Iconer.Net.IoContext;
import Iconer.Net.Socket;
import <cstddef>;
import <cstdint>;
import <memory>;
import <span>;
import <array>;
import <atomic>;

export namespace iconer::net
{
	class [[nodiscard]] TcpReceiver
	{
	public:
		using this_class = TcpReceiver;

		static inline constexpr std::uint32_t maxRecvSize = 512;

		iconer::net::Socket* mySocket;
		alignas(std::hardware_constructive_interference_size) std::array<std::byte, maxRecvSize> recvBuffer;
		std::atomic_int32_t recvBytes;

		constexpr TcpReceiver(iconer::net::Socket& socket, std::uint32_t buffer_size)
			: mySocket(std::addressof(socket))
			, recvBuffer()
			, recvBytes()
		{}

		[[nodiscard]] iconer::util::Expected<int, iconer::net::ErrorCode> Receive();

		iconer::net::IoResult BeginOptainMemory(iconer::net::IoContext& context);
		void EndOptainMemory(iconer::net::IoContext& context) noexcept;
		
		iconer::net::IoResult BeginReceive(iconer::net::IoContext& context);
		bool EndReceive(iconer::net::IoContext& context, const std::uint32_t bytes) noexcept;

		iconer::net::IoResult BeginReceive(iconer::net::IoContext* context);
		bool EndReceive(iconer::net::IoContext* context, const std::uint32_t bytes) noexcept;

		iconer::net::IoResult BeginClose(iconer::net::IoContext& context);
		iconer::net::IoResult BeginClose(iconer::net::IoContext* context);
		void EndClose();

		void PullReceivedData(const size_t& size);
		[[nodiscard]]
		std::unique_ptr<std::byte[]> AcquireReceivedData(const size_t& size);

		[[nodiscard]] std::span<std::byte> GetReceiveBuffer() noexcept;
		[[nodiscard]] std::span<std::byte> GetCurrentReceiveBuffer() noexcept;

		[[nodiscard]]
		constexpr iconer::net::Socket& GetSocket() noexcept
		{
			return *mySocket;
		}
		
		[[nodiscard]]
		constexpr const iconer::net::Socket& GetSocket() const noexcept
		{
			return *mySocket;
		}

	private:
		TcpReceiver(const TcpReceiver&) = delete;
		TcpReceiver& operator=(const TcpReceiver&) = delete;
	};
}
