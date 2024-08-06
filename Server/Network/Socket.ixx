export module Iconer.Net.Socket;
import Iconer.Utility.TypeTraits;
//import Iconer.Utility.Property;
export import :SocketOption;
export import :SocketCategory;
export import Iconer.Net.ErrorCode;
export import Iconer.Net.InternetProtocol;
export import Iconer.Net.IpAddressFamily;
export import Iconer.Net.IoResult;
import <cstddef>;
import <cstdint>;
import <span>;

export struct _OVERLAPPED;

export namespace iconer::net
{
	class IpAddress;
	class EndPoint;
	class IoContext;

	class [[nodiscard]] Socket
	{
	public:
		using SyncIoResult = iconer::util::Expected<int, iconer::net::ErrorCode>;

		static inline constexpr std::uintptr_t invalidHandle = static_cast<std::uintptr_t>(-1);

		//iconer::util::Property<bool, Socket> reuseAddress{ this, &Socket::ReuseAddressImplementation, false };
		//iconer::util::Property<bool, Socket> tcpNoDelay{ this, &Socket::TcpNoDelayImplementation, false };

		explicit constexpr Socket() noexcept
			: myHandle(invalidHandle)
			, myProtocol(iconer::net::InternetProtocol::Unknown)
			, myFamily(iconer::net::IpAddressFamily::Unknown)
		{}

		IoResult Bind(const iconer::net::IpAddress& address, std::uint16_t port) const noexcept;
		IoResult Bind(iconer::net::IpAddress&& address, std::uint16_t port) const noexcept;
		IoResult Bind(const iconer::net::EndPoint& endpoint) const noexcept;
		IoResult Bind(iconer::net::EndPoint&& endpoint) const noexcept;
		IoResult BindToAny(std::uint16_t port) const noexcept;
		IoResult BindToHost(std::uint16_t port) const noexcept;

		IoResult Open() const noexcept;
		IoResult Connect(const iconer::net::IpAddress& address, std::uint16_t port) const noexcept;
		IoResult Connect(iconer::net::IpAddress&& address, std::uint16_t port) const noexcept;
		IoResult Connect(const iconer::net::EndPoint& endpoint) const noexcept;
		IoResult Connect(iconer::net::EndPoint&& endpoint) const noexcept;
		IoResult ConnectToHost(std::uint16_t port) const noexcept;

		IoResult SetOption(SocketOption option, const void* opt_buffer, const size_t opt_size) const noexcept;

		[[nodiscard]] bool ReusableAddress() const noexcept;
		IoResult ReusableAddress(bool flag) noexcept;

		[[nodiscard]] bool IsTcpNoDelay() const noexcept;
		IoResult SetTcpNoDelay(bool flag) noexcept;

		[[nodiscard]]
		iconer::util::Expected<Socket, iconer::net::ErrorCode> Accept() const noexcept;
		[[nodiscard]]
		iconer::util::Expected<Socket, iconer::net::ErrorCode> Accept(iconer::net::EndPoint& endpoint) const noexcept;

		IoResult OptainReceiveMemory(IoContext& context, std::span<std::byte> memory) const;
		IoResult OptainReceiveMemory(IoContext& context, std::byte* memory, size_t size) const;
		IoResult OptainReceiveMemory(IoContext* context, std::span<std::byte> memory) const;
		IoResult OptainReceiveMemory(IoContext* context, std::byte* memory, size_t size) const;

		/// <summary>
		/// Reserve an acceptance of <paramref name="client"/>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="context"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context"/>
		/// <param name="client">- Client socket</param>
		/// <exception cref="std::system_error"/>
		IoResult BeginAccept(iconer::net::IoContext& context, const Socket& client) const;
		/// <summary>
		/// Reserve an acceptance of <paramref name="client"/>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="context"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context"/>
		/// <param name="client">- Client socket</param>
		/// <param name="accept_buffer">- Received datas from beginning would be written here</param>
		/// <exception cref="std::system_error"/>
		IoResult BeginAccept(iconer::net::IoContext& context, const Socket& client, std::span<std::byte> accept_buffer) const;
		/// <summary>
		/// Reserve an acceptance of <paramref name="client"/>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="context"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context"/>
		/// <param name="client">- Client socket</param>
		/// <exception cref="std::system_error"/>
		IoResult BeginAccept(iconer::net::IoContext* context, const Socket& client) const;
		/// <summary>
		/// Reserve an acceptance of <paramref name="client"/>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="context"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context"/>
		/// <param name="client">- Client socket</param>
		/// <param name="accept_buffer">- Received datas from beginning would be written here</param>
		/// <exception cref="std::system_error"/>
		IoResult BeginAccept(iconer::net::IoContext* context, const Socket& client, std::span<std::byte> accept_buffer) const;
		/// <summary>
		/// Finish the acceptance of <paramref name="client"/>
		/// <para>It should be called by client</para>
		/// <para>-------------------------------------------------------------------------------</para>
		/// </summary>
		/// <param name="listener">- The listener socket</param>
		IoResult EndAccept(const Socket& listener) const noexcept;

		/// <summary>
		/// Send data through this socket
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="memory">- The send data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		SyncIoResult Send(std::span<const std::byte> memory) const;
		/// <summary>
		/// Send data through this socket
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="memory">- The send data buffer</param>
		/// <param name="size">- Size of the send data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		SyncIoResult Send(std::span<const std::byte> memory, size_t size) const;
		/// <summary>
		/// Send data through this socket
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="memory">- A pointer to the send data buffer</param>
		/// <param name="size">- Size of the send data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		SyncIoResult Send(const std::byte* const& memory, size_t size) const;
		/// <summary>
		/// Send data through this socket
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="memory">- The send data buffer</param>
		/// <param name="outpin">- The failsafe output pin for error code</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		bool Send(std::span<const std::byte> memory, iconer::net::ErrorCode& outpin) const;
		/// <summary>
		/// Send data through this socket
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="memory">- The send data buffer</param>
		/// <param name="size">- Size of the send data buffer</param>
		/// <param name="outpin">- The failsafe output pin for error code</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		bool Send(std::span<const std::byte> memory, size_t size, iconer::net::ErrorCode& outpin) const;
		/// <summary>
		/// Send data through this socket
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="memory">- A pointer to the send data buffer</param>
		/// <param name="size">- Size of the send data buffer</param>
		/// <param name="outpin">- The failsafe output pin for error code</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		bool Send(const std::byte* const& memory, size_t size, iconer::net::ErrorCode& outpin) const;

		/// <summary>
		/// Send data through this socket
		/// <para><b>(Asynchronous IO)</b></para>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context">- The io context object</param>
		/// <param name="memory">- The send data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		IoResult Send(iconer::net::IoContext& context, std::span<const std::byte> memory) const;
		/// <summary>
		/// Send data through this socket
		/// <para><b>(Asynchronous IO)</b></para>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context">- The io context object</param>
		/// <param name="memory">- The send data buffer</param>
		/// <param name="size">- Size of the send data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		IoResult Send(iconer::net::IoContext& context, std::span<const std::byte> memory, size_t size) const;
		/// <summary>
		/// Send data through this socket
		/// <para><b>(Asynchronous IO)</b></para>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context">- The io context object</param>
		/// <param name="memory">- A pointer to the send data buffer</param>
		/// <param name="size">- Size of the send data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		IoResult Send(iconer::net::IoContext& context, const std::byte* const& memory, size_t size) const;
		/// <summary>
		/// Send data through this socket
		/// <para><b>(Asynchronous IO)</b></para>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context">- The io context object</param>
		/// <param name="memory">- The send data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		bool Send(iconer::net::IoContext& context, std::span<const std::byte> memory, iconer::net::ErrorCode& outpin) const;
		/// <summary>
		/// Send data through this socket
		/// <para><b>(Asynchronous IO)</b></para>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context">- The io context object</param>
		/// <param name="memory">- The send data buffer</param>
		/// <param name="size">- Size of the send data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		bool Send(iconer::net::IoContext& context, std::span<const std::byte> memory, size_t size, iconer::net::ErrorCode& outpin) const;
		/// <summary>
		/// Send data through this socket
		/// <para><b>(Asynchronous IO)</b></para>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context">- The io context object</param>
		/// <param name="memory">- A pointer to the send data buffer</param>
		/// <param name="size">- Size of the send data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		bool Send(iconer::net::IoContext& context, const std::byte* const& memory, size_t size, iconer::net::ErrorCode& outpin) const;

		/// <summary>
		/// Receive data into the memory buffer
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="memory">- The receive data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		SyncIoResult Receive(std::span<std::byte> memory) const;
		/// <summary>
		/// Receive data into the memory buffer
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="memory">- The receive data buffer</param>
		/// <param name="size">- Size of the receive data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		SyncIoResult Receive(std::span<std::byte> memory, size_t size) const;
		/// <summary>
		/// Receive data into the memory buffer
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="memory">- A pointer to the receive data buffer</param>
		/// <param name="size">- Size of the receive data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		SyncIoResult Receive(std::byte* memory, size_t size) const;
		/// <summary>
		/// Receive data into the memory buffer
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="memory">- The receive data buffer</param>
		/// <param name="outpin">- The failsafe output pin for error code</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		bool Receive(std::span<std::byte> memory, ErrorCode& outpin) const;
		/// <summary>
		/// Receive data into the memory buffer
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="memory">- The receive data buffer</param>
		/// <param name="size">- Size of the receive data buffer</param>
		/// <param name="outpin">- The failsafe output pin for error code</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		bool Receive(std::span<std::byte> memory, size_t size, ErrorCode& outpin) const;
		/// <summary>
		/// Receive data into the memory buffer
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="memory">- A pointer to the receive data buffer</param>
		/// <param name="size">- Size of the receive data buffer</param>
		/// <param name="outpin">- The failsafe output pin for error code</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		bool Receive(std::byte* memory, size_t size, ErrorCode& outpin) const;

		/// <summary>
		/// Receive data into the memory buffer
		/// <para><b>(Asynchronous IO)</b></para>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context">- The io context object</param>
		/// <param name="memory">- The receive data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		IoResult Receive(IoContext& context, std::span<std::byte> memory) const;
		/// <summary>
		/// Receive data into the memory buffer
		/// <para><b>(Asynchronous IO)</b></para>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="memory">- The receive data buffer</param>
		/// <param name="size">- Size of the receive data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		IoResult Receive(IoContext& context, std::span<std::byte> memory, size_t size) const;
		/// <summary>
		/// Receive data into the memory buffer
		/// <para><b>(Asynchronous IO)</b></para>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context">- The io context object</param>
		/// <param name="memory">- A pointer to the receive data buffer</param>
		/// <param name="size">- Size of the receive data buffer</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		IoResult Receive(IoContext& context, std::byte* memory, size_t size) const;
		/// <summary>
		/// Receive data into the memory buffer
		/// <para><b>(Asynchronous IO)</b></para>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context">- The io context object</param>
		/// <param name="memory">- The receive data buffer</param>
		/// <param name="outpin">- The failsafe output pin for error code</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		bool Receive(IoContext& context, std::span<std::byte> memory, ErrorCode& outpin) const;
		/// <summary>
		/// Receive data into the memory buffer
		/// <para><b>(Asynchronous IO)</b></para>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context">- The io context object</param>
		/// <param name="memory">- The receive data buffer</param>
		/// <param name="size">- Size of the receive data buffer</param>
		/// <param name="outpin">- The failsafe output pin for error code</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		bool Receive(IoContext& context, std::span<std::byte> memory, size_t size, ErrorCode& outpin) const;
		/// <summary>
		/// Receive data into the memory buffer
		/// <para><b>(Asynchronous IO)</b></para>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::system_error"/> when <paramref name="memory"/> is <value>nullptr</value></para>
		/// </summary>
		/// <param name="context">- The io context object</param>
		/// <param name="memory">- A pointer to the receive data buffer</param>
		/// <param name="size">- Size of the receive data buffer</param>
		/// <param name="outpin">- The failsafe output pin for error code</param>
		/// <exception cref="std::overflow_error"/>
		/// <exception cref="std::system_error"/>
		bool Receive(IoContext& context, std::byte* memory, size_t size, ErrorCode& outpin) const;

		IoResult Close() const noexcept;
		bool Close(iconer::net::ErrorCode& error_code) const noexcept;

		IoResult AsyncClose(iconer::net::IoContext& context) const noexcept;
		IoResult AsyncClose(iconer::net::IoContext* const context) const noexcept;

		bool AsyncClose(iconer::net::IoContext& context, iconer::net::ErrorCode& error_code) const noexcept;
		bool AsyncClose(iconer::net::IoContext* const context, iconer::net::ErrorCode& error_code) const noexcept;

		[[nodiscard]]
		constexpr const std::uintptr_t& GetNativeHandle() const& noexcept
		{
			return myHandle;
		}

		[[nodiscard]]
		constexpr std::uintptr_t&& GetNativeHandle() && noexcept
		{
			return std::move(myHandle);
		}

		[[nodiscard]]
		constexpr iconer::net::InternetProtocol GetInternetProtocol() const noexcept
		{
			return myProtocol;
		}

		[[nodiscard]]
		constexpr iconer::net::IpAddressFamily GetIpAddressFamily() const noexcept
		{
			return myFamily;
		}

		[[nodiscard]]
		constexpr explicit operator bool() const noexcept
		{
			return (0 != myHandle and invalidHandle != myHandle);
		}

		constexpr Socket(Socket&& other) noexcept
			: myHandle(std::exchange(other.myHandle, invalidHandle))
			, myProtocol(other.myProtocol)
			, myFamily(other.myFamily)
		{}

		constexpr Socket& operator=(Socket&& other) noexcept
		{
			myHandle = std::exchange(other.myHandle, invalidHandle);
			myProtocol = other.myProtocol;
			myFamily = other.myFamily;

			return *this;
		}

		[[nodiscard]]
		constexpr bool operator==(const Socket&) const noexcept = default;

		[[nodiscard]]
		static iconer::util::Expected<Socket, iconer::net::ErrorCode> Create(iconer::net::SocketCategory type, iconer::net::InternetProtocol protocol, iconer::net::IpAddressFamily family) noexcept;

		[[nodiscard]]
		static bool TryCreate(iconer::net::SocketCategory type, iconer::net::InternetProtocol protocol, iconer::net::IpAddressFamily family, iconer::net::Socket& result, iconer::net::ErrorCode& outpin) noexcept;

		[[nodiscard]]
		static Socket CreateTcpSocket(iconer::net::SocketCategory type, iconer::net::IpAddressFamily family = iconer::net::IpAddressFamily::IPv4) noexcept;

		[[nodiscard]]
		static Socket CreateUdpSocket(iconer::net::SocketCategory type, iconer::net::IpAddressFamily family = iconer::net::IpAddressFamily::IPv4) noexcept;

	private:
		std::uintptr_t myHandle;
		iconer::net::InternetProtocol myProtocol;
		iconer::net::IpAddressFamily myFamily;

		using AsyncAcceptFunction = std_function_t<int, std::uintptr_t, std::uintptr_t, void*, unsigned long, unsigned long, unsigned long, unsigned long*, struct _OVERLAPPED*>;

		using AsyncCloseFunction = std_function_t<int, std::uintptr_t, void*, unsigned long, unsigned long, struct _OVERLAPPED*, struct _TRANSMIT_FILE_BUFFERS*, unsigned long>;

		struct ErrorTransfer
		{
			constexpr iconer::util::Expected<bool, iconer::net::ErrorCode> operator()(iconer::net::ErrorCode&& error_code) const noexcept
			{
				errorOutput = std::exchange(error_code, {});
				return false;
			}

			iconer::net::ErrorCode& errorOutput;
		};

		void ReuseAddressImplementation(bool flag) noexcept;
		void TcpNoDelayImplementation(bool flag) noexcept;

		static inline constinit AsyncAcceptFunction asyncAcceptFnPtr = nullptr;
		static inline constinit AsyncCloseFunction asyncTransmitFnPtr = nullptr;

		Socket(std::uintptr_t sock, iconer::net::InternetProtocol protocol, iconer::net::IpAddressFamily family) noexcept;

		static void InternalFunctionInitializer(std::uintptr_t socket);

		Socket(const Socket&) = delete;
		Socket& operator=(const Socket&) = delete;
	};
}