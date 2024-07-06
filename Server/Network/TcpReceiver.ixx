export module Iconer.Net.TcpReceiver;
import Iconer.Net.ErrorCode;
import Iconer.Net.IoResult;
import Iconer.Net.IoContext;
import <cstddef>;
import <cstdint>;
import <memory>;
import <span>;
import <vector>;
import <atomic>;

export namespace iconer::net
{
	class [[nodiscard]] Socket;

	class [[nodiscard]] TcpReceiver
	{
	public:
		using this_class = TcpReceiver;

		iconer::net::Socket* mySocket;
		std::uint32_t maxRecvSize;
		std::atomic_uint32_t recvBytes;
		alignas(std::hardware_constructive_interference_size) std::vector<std::byte> recvBuffer;

		constexpr TcpReceiver(iconer::net::Socket* socket, std::uint32_t buffer_size)
			: mySocket(socket)
			, recvBuffer(buffer_size, std::byte{}), maxRecvSize(buffer_size)
			, recvBytes()
		{}

		~TcpReceiver() noexcept;

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

		[[nodiscard]]
		std::unique_ptr<std::byte[]> AcquireReceivedData(size_t size);

		[[nodiscard]] std::span<std::byte> GetReceiveBuffer() noexcept;
		[[nodiscard]] std::span<const std::byte> GetReceiveBuffer() const noexcept;
		[[nodiscard]] std::span<std::byte> GetCurrentReceiveBuffer() noexcept;

		[[nodiscard]]
		constexpr const iconer::net::Socket& GetSocket() const noexcept
		{
			return *mySocket;
		}

		TcpReceiver(TcpReceiver&&) = default;
		TcpReceiver& operator=(TcpReceiver&&) = default;

	private:
		TcpReceiver(const TcpReceiver&) = delete;
		TcpReceiver& operator=(const TcpReceiver&) = delete;
	};
}
