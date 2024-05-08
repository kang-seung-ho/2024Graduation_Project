module;
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#include <future>

module Iconer.Net.Socket;
import Iconer.Net.EndPoint;

::RIO_BUF buf;
static inline constexpr ::SOCKET InvalidSocket = INVALID_SOCKET;
constinit static inline ::SOCKET internalSocket = InvalidSocket;
constinit static inline std::once_flag internalInitFlag{};

constinit static inline ::WSAOVERLAPPED rioContext{};
constinit static inline ::RIO_EXTENSION_FUNCTION_TABLE rioFunctions{};

constinit static inline ::LPFN_ACCEPTEX fnAcceptEx = nullptr;
constinit static inline ::LPFN_TRANSMITFILE fnTransmitFile = nullptr;
static inline constexpr unsigned long DEFAULT_ACCEPT_SIZE = sizeof(SOCKADDR_IN) + 16UL;

static void CALLBACK rioRoutine(const ::DWORD err, const ::DWORD bytes, ::LPWSAOVERLAPPED ctx, const ::DWORD flags)
{
	if (0 != err)
	{
		//std::println("Socket error: {}", err);
	}
}
void SocketFunctionInitializer(const iconer::net::Socket::HandleType& sock);

::SOCKADDR_STORAGE SerializeEndpoint(const iconer::net::EndPoint& endpoint) noexcept;
::SOCKADDR_STORAGE SerializeEndpoint(iconer::net::EndPoint&& endpoint) noexcept;
iconer::net::Socket::ActionResult RawGetOption(const iconer::net::Socket::HandleType& sock, int option) noexcept;

struct [[nodiscard]] SerializedIpAddress
{
	char data[64];
};

SerializedIpAddress
SerializeIpAddress(const iconer::net::IpAddress& ip_address)
noexcept
{
	SerializedIpAddress result{};

	::inet_pton((int)ip_address.addressFamily
		, ip_address.addressString.data()
		, std::addressof(result));

	return result;
}

bool
TrySerializeIpAddress(const iconer::net::IpAddress& ip_address, SerializedIpAddress& out)
noexcept
{
	SerializedIpAddress result{};

	if (1 != ::inet_pton((int)ip_address.addressFamily
		, ip_address.addressString.data()
		, std::addressof(result)))
	{
		return false;
	}
	else
	{
		out = std::move(result);
		return true;
	}
}

bool
TrySerializeIpAddress(const iconer::net::IpAddress& ip_address, void* const& out)
noexcept
{
	if (1 != ::inet_pton((int)ip_address.addressFamily
		, ip_address.addressString.data()
		, out))
	{
		return false;
	}
	else
	{
		return true;
	}
}

iconer::net::Socket::Socket()
noexcept
	: Handler(INVALID_SOCKET)
	, myProtocol(InternetProtocol::Unknown), myFamily(IpAddressFamily::Unknown)
	, IsAddressReusable(false, DelegateAddressReusable{ INVALID_SOCKET })
{
}

iconer::net::Socket::Socket(iconer::net::Socket::HandleType sock, iconer::net::InternetProtocol protocol, iconer::net::IpAddressFamily family)
noexcept
	: Handler(sock)
	, myProtocol(protocol), myFamily(family)
	, IsAddressReusable(false, DelegateAddressReusable{ sock })
{
	std::call_once(internalInitFlag, ::SocketFunctionInitializer, sock);
}

iconer::net::Socket::ActionResult
iconer::net::Socket::Bind(const iconer::net::EndPoint& endpoint)
const noexcept
{
	SOCKADDR_STORAGE sockaddr = SerializeEndpoint(endpoint);
	SOCKADDR_STORAGE* ptr = std::addressof(sockaddr);

	if (0 != ::bind(Super::GetHandle(), reinterpret_cast<const SOCKADDR*>(ptr), sizeof(sockaddr)))
	{
		return AcquireNetworkError();
	}

	return std::nullopt;
}

iconer::net::Socket::ActionResult
iconer::net::Socket::Bind(iconer::net::EndPoint&& endpoint)
const noexcept
{
	SOCKADDR_STORAGE sockaddr = SerializeEndpoint(std::move(endpoint));
	SOCKADDR_STORAGE* ptr = std::addressof(sockaddr);

	if (0 != ::bind(Super::GetHandle(), reinterpret_cast<const SOCKADDR*>(ptr), sizeof(sockaddr)))
	{
		return AcquireNetworkError();
	}

	return std::nullopt;
}

iconer::net::Socket::ActionResult
iconer::net::Socket::Open()
const noexcept
{
	const int open = ::listen(Super::GetHandle(), SOMAXCONN);
	if (SOCKET_ERROR == open)
	{
		return AcquireNetworkError();
	}

	return std::nullopt;
}

iconer::net::Socket::ActionResult
iconer::net::Socket::Connect(const iconer::net::EndPoint& endpoint)
const noexcept
{
	SOCKADDR_STORAGE sockaddr = SerializeEndpoint(endpoint);
	SOCKADDR_STORAGE* ptr = std::addressof(sockaddr);

	if (0 != ::WSAConnect(Super::GetHandle()
		, reinterpret_cast<const SOCKADDR*>(ptr), sizeof(sockaddr)
		, nullptr, nullptr
		, nullptr, nullptr))
	{
		auto error_code = AcquireNetworkError();
		if (error_code == ErrorCode::NonBlockedOperation)
		{
			return std::nullopt;
		}
		else
		{
			return error_code;
		}
	}

	return std::nullopt;
}

iconer::net::Socket::ActionResult
iconer::net::Socket::Connect(iconer::net::EndPoint&& endpoint)
const noexcept
{
	SOCKADDR_STORAGE sockaddr = SerializeEndpoint(std::move(endpoint));
	SOCKADDR_STORAGE* ptr = std::addressof(sockaddr);

	if (0 != ::WSAConnect(Super::GetHandle()
		, reinterpret_cast<const SOCKADDR*>(ptr), sizeof(sockaddr)
		, nullptr, nullptr
		, nullptr, nullptr))
	{
		auto error_code = AcquireNetworkError();
		if (error_code == ErrorCode::NonBlockedOperation)
		{
			return std::nullopt;
		}
		else
		{
			return error_code;
		}
	}

	return std::nullopt;
}

bool
iconer::net::Socket::Close()
const noexcept
{
	if (IsAvailable())
	{
		if (IsAddressReusable)
		{
			return (1 == ::fnTransmitFile(Super::GetHandle(), nullptr, 0, 0, nullptr, nullptr, TF_DISCONNECT | TF_REUSE_SOCKET));
		}
		else
		{
			return (0 == ::closesocket(Super::GetHandle()));
		}
	}
	else
	{
		return false;
	}
}

bool
iconer::net::Socket::Close(iconer::net::ErrorCode& error_code)
const noexcept
{
	if (IsAvailable())
	{
		if (IsAddressReusable)
		{
			if (1 == ::fnTransmitFile(Super::GetHandle(), nullptr, 0, 0, nullptr, nullptr, TF_DISCONNECT | TF_REUSE_SOCKET))
			{
				return true;
			}
			else
			{
				error_code = AcquireNetworkError();
				return false;
			}
		}
		else
		{
			if (0 == ::closesocket(Super::GetHandle()))
			{
				return true;
			}
			else
			{
				error_code = AcquireNetworkError();
				return false;
			}
		}
	}
	else
	{
		error_code = ErrorCode::NotASocket;
		return false;
	}
}

bool
iconer::net::Socket::CloseAsync(iconer::net::IoContext& context)
const noexcept
{
	return CloseAsync(std::addressof(context));
}

bool
iconer::net::Socket::CloseAsync(iconer::net::IoContext& context, iconer::net::ErrorCode& error_code)
const noexcept
{
	return CloseAsync(std::addressof(context), error_code);
}

bool
iconer::net::Socket::CloseAsync(iconer::net::IoContext* const context)
const noexcept
{
	if (IsAvailable())
	{
		auto* ctx = static_cast<::LPWSAOVERLAPPED>(context);
		if (IsAddressReusable)
		{
			return (1 == ::fnTransmitFile(Super::GetHandle(), nullptr, 0, 0, ctx, nullptr, TF_DISCONNECT | TF_REUSE_SOCKET));
		}
		else
		{
			return (1 == ::fnTransmitFile(Super::GetHandle(), nullptr, 0, 0, ctx, nullptr, TF_DISCONNECT));
		}
	}
	else
	{
		return false;
	}
}

bool
iconer::net::Socket::CloseAsync(iconer::net::IoContext* const context, iconer::net::ErrorCode& error_code)
const noexcept
{
	if (IsAvailable())
	{
		if (CloseAsync(context))
		{
			return true;
		}
		else
		{
			error_code = AcquireNetworkError();
			return false;
		}
	}
	else
	{
		error_code = ErrorCode::NotASocket;
		return false;
	}
}

iconer::net::Socket::ActionResult
iconer::net::Socket::BeginAccept(iconer::net::IoContext& context, iconer::net::Socket& client)
const
{
	return BeginAccept(std::addressof(context), client);
}

iconer::net::Socket::ActionResult
iconer::net::Socket::BeginAccept(iconer::net::IoContext& context, Socket& client, std::span<std::byte> accept_buffer)
const
{
	return BeginAccept(std::addressof(context), client, std::move(accept_buffer));
}

iconer::net::Socket::ActionResult
iconer::net::Socket::BeginAccept(iconer::net::IoContext* context, iconer::net::Socket& client)
const
{
	char temp_buffer[::DEFAULT_ACCEPT_SIZE * 2]{};
	if (not IsAvailable())
	{
		return AcquireNetworkError();
	}

	::DWORD result_bytes{};

	if (1 == ::fnAcceptEx(Super::GetHandle(), client.GetHandle()
		, temp_buffer, 0UL
		, ::DEFAULT_ACCEPT_SIZE
		, ::DEFAULT_ACCEPT_SIZE
		, std::addressof(result_bytes)
		, static_cast<::LPWSAOVERLAPPED>(context))
		)
	{
		return std::nullopt;
	}
	else
	{
		if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation)
		{
			return std::move(error);
		}
		else
		{
			return std::nullopt;
		}
	}
}

iconer::net::Socket::ActionResult
iconer::net::Socket::BeginAccept(iconer::net::IoContext* context, iconer::net::Socket& client, std::span<std::byte> accept_buffer)
const
{
	if (not IsAvailable())
	{
		return AcquireNetworkError();
	}

	::DWORD result_bytes{};

	if (1 == ::fnAcceptEx(Super::GetHandle(), client.GetHandle()
		, accept_buffer.data(), static_cast<::DWORD>(accept_buffer.size_bytes())
		, ::DEFAULT_ACCEPT_SIZE
		, ::DEFAULT_ACCEPT_SIZE
		, std::addressof(result_bytes)
		, static_cast<::LPWSAOVERLAPPED>(context))
		)
	{
		return std::nullopt;
	}
	else
	{
		if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation)
		{
			return std::move(error);
		}
		else
		{

			return std::nullopt;
		}
	}
}

iconer::net::Socket::ActionResult
iconer::net::Socket::EndAccept(Socket& listener)
const noexcept
{
	//::setsockopt(Super::GetHandle(), SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<const char*>(), sizeof(HandleType));

	return SetOption(SocketOptions::UpdateContext, std::addressof(listener.GetHandle()), sizeof(HandleType));
}

iconer::net::Socket::IoResult
iconer::net::Socket::SendTo(const iconer::net::EndPoint& ep, std::span<const std::byte> memory)
const noexcept
{
	::SOCKADDR_STORAGE sockaddr = SerializeEndpoint(ep);
	::SOCKADDR_STORAGE* ptr = std::addressof(sockaddr);

	if (int bytes = ::sendto(Super::GetHandle()
		, reinterpret_cast<const char*>(memory.data()), static_cast<int>(memory.size_bytes())
		, 0
		, reinterpret_cast<const SOCKADDR*>(ptr), sizeof(sockaddr)); SOCKET_ERROR != bytes)
	{
		return bytes;
	}
	else
	{
		return std::unexpected{ AcquireNetworkError() };
	}
}

iconer::net::Socket::IoResult
iconer::net::Socket::SendTo(const iconer::net::EndPoint& ep, std::span<const std::byte> memory, size_t size) const noexcept
{
	::SOCKADDR_STORAGE sockaddr = SerializeEndpoint(ep);
	::SOCKADDR_STORAGE* ptr = std::addressof(sockaddr);

	if (int bytes = ::sendto(Super::GetHandle()
		, reinterpret_cast<const char*>(memory.data()), static_cast<int>(size)
		, 0
		, reinterpret_cast<const SOCKADDR*>(ptr), sizeof(sockaddr)); SOCKET_ERROR != bytes)
	{
		return bytes;
	}
	else
	{
		return std::unexpected{ AcquireNetworkError() };
	}
}

iconer::net::Socket::IoResult
iconer::net::Socket::SendTo(const iconer::net::EndPoint& ep, const std::byte* const& memory, size_t size)
const noexcept
{
	::SOCKADDR_STORAGE sockaddr = SerializeEndpoint(ep);
	::SOCKADDR_STORAGE* ptr = std::addressof(sockaddr);

	if (int bytes = ::sendto(Super::GetHandle()
		, reinterpret_cast<const char*>(memory), static_cast<int>(size)
		, 0
		, reinterpret_cast<const SOCKADDR*>(ptr), sizeof(sockaddr)); SOCKET_ERROR != bytes)
	{
		return bytes;
	}
	else
	{
		return std::unexpected{ AcquireNetworkError() };
	}
}

bool
iconer::net::Socket::SendTo(const EndPoint& ep, std::span<const std::byte> memory, ErrorCode& error_code)
const noexcept
{
	return SendTo(ep, memory).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

bool
iconer::net::Socket::SendTo(const EndPoint& ep, std::span<const std::byte> memory, size_t size, ErrorCode& error_code) 
const noexcept
{
	return SendTo(ep, memory, std::move(size)).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

bool
iconer::net::Socket::SendTo(const EndPoint& ep, const std::byte* const& memory, size_t size, ErrorCode& error_code)
const noexcept
{
	return SendTo(ep, memory, std::move(size)).transform_error(ErrorTransfer{ error_code }).value_or(true);
}

iconer::net::Socket
iconer::net::Socket::Create(IoCategory type
	, const InternetProtocol& protocol
	, const IpAddressFamily& family)
	noexcept
{
	const auto flags = std::to_underlying(type);

	HandleType result;
	switch (protocol)
	{
		case InternetProtocol::TCP:
		{
			result = ::WSASocket(static_cast<int>(family), SOCK_STREAM, ::IPPROTO::IPPROTO_TCP, nullptr, 0, flags);
		}
		break;

		case InternetProtocol::UDP:
		{
			result = ::WSASocket(static_cast<int>(family), SOCK_DGRAM, ::IPPROTO::IPPROTO_UDP, nullptr, 0, flags);
		}
		break;

		case InternetProtocol::Unknown:
		{
			return Socket{};
		}
	}

	return Socket(result, protocol, family);
}

iconer::net::Socket
iconer::net::Socket::Create(iconer::net::IoCategory type
	, const iconer::net::InternetProtocol& protocol
	, const iconer::net::IpAddressFamily& family
	, iconer::net::ErrorCode& error_code)
	noexcept
{
	if (Socket result = Create(type, protocol, family); result.IsAvailable())
	{
		return result;
	}
	else
	{
		error_code = AcquireNetworkError();
		return Socket{};
	}
}

bool
iconer::net::Socket::TryCreate(iconer::net::IoCategory type
	, const iconer::net::InternetProtocol& protocol
	, const iconer::net::IpAddressFamily& family
	, iconer::net::Socket& out)
	noexcept
{
	if (Socket result = Create(type, protocol, family); result.IsAvailable())
	{
		out = std::exchange(result, Socket{});

		return true;
	}
	else
	{
		return false;
	}
}

bool
iconer::net::Socket::TryCreate(iconer::net::IoCategory type
	, const iconer::net::InternetProtocol& protocol
	, const iconer::net::IpAddressFamily& family
	, iconer::net::Socket& out
	, iconer::net::ErrorCode& error_code)
	noexcept
{
	if (Socket result = Create(type, protocol, family); result.IsAvailable())
	{
		out = std::exchange(result, Socket{});

		return true;
	}
	else
	{
		error_code = AcquireNetworkError();
		return false;
	}
}

iconer::net::Socket::FactoryResult
iconer::net::Socket::TryCreate(iconer::net::IoCategory type
	, const iconer::net::InternetProtocol& protocol
	, const iconer::net::IpAddressFamily& family)
	noexcept
{
	if (Socket result = Create(type, protocol, family); result.IsAvailable())
	{
		return std::exchange(result, Socket{});
	}
	else
	{
		return std::unexpected{ AcquireNetworkError() };
	}
}

iconer::net::Socket
iconer::net::Socket::CreateTcpSocket(iconer::net::IoCategory type, const iconer::net::IpAddressFamily& family)
noexcept
{
	return Create(type, InternetProtocol::TCP, family);
}

iconer::net::Socket
iconer::net::Socket::CreateUdpSocket(iconer::net::IoCategory type, const iconer::net::IpAddressFamily& family)
noexcept
{
	return Create(type, InternetProtocol::UDP, family);
}

void
SocketFunctionInitializer(const iconer::net::Socket::HandleType& sock)
{
	::GUID fntable_id = WSAID_MULTIPLE_RIO;
	::DWORD temp_bytes = 0;

	::GUID* fntable_addr = std::addressof(fntable_id);
	::DWORD* bytes_addr = std::addressof(temp_bytes);

#if _DEBUG
	int result =
#endif // _DEBUG
		::WSAIoctl(sock, SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER
		, fntable_addr, sizeof(GUID)
		, reinterpret_cast<void**>(std::addressof(rioFunctions)), sizeof(rioFunctions)
		, bytes_addr
		, std::addressof(rioContext), ::rioRoutine);

	fntable_id = WSAID_ACCEPTEX;
#if _DEBUG
	result =
#endif // _DEBUG
		::WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER
		, fntable_addr, sizeof(GUID)
		, std::addressof(fnAcceptEx), sizeof(fnAcceptEx)
		, bytes_addr
		, nullptr, nullptr);

	fntable_id = WSAID_TRANSMITFILE;
#if _DEBUG
	result =
#endif // _DEBUG
		::WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER
		, fntable_addr, sizeof(GUID)
		, std::addressof(fnTransmitFile), sizeof(fnTransmitFile)
		, bytes_addr
		, nullptr, nullptr);
}

[[nodiscard]]
::SOCKADDR_STORAGE
SerializeEndpoint(const iconer::net::EndPoint& endpoint)
noexcept
{
	const auto& ip = endpoint.IpAddress();
	const auto& port = endpoint.Port();

	::SOCKADDR_STORAGE result{};
	::SOCKADDR_STORAGE* sockaddr_ptr = std::addressof(result);

	switch (endpoint.AddressFamily())
	{
		case iconer::net::IpAddressFamily::IPv4:
		{
			::IN_ADDR sk_addr{};
			if (not TrySerializeIpAddress(ip, std::addressof(sk_addr.s_addr)))
			{
				break;
			}

			::SOCKADDR_IN ipv4_addr
			{
				.sin_family = (std::uint16_t)iconer::net::IpAddressFamily::IPv4,
				.sin_port = port,
				.sin_addr = std::move(sk_addr),
			};

			result = *reinterpret_cast<const ::SOCKADDR_STORAGE*>(std::addressof(ipv4_addr));
		}
		break;

		case iconer::net::IpAddressFamily::IPv6:
		{
			::IN6_ADDR sk_addr{};
			if (not TrySerializeIpAddress(ip, std::addressof(sk_addr.s6_addr)))
			{
				break;
			}

			::SOCKADDR_IN6 ipv6_addr
			{
				.sin6_family = (std::uint16_t)iconer::net::IpAddressFamily::IPv6,
				.sin6_port = port,
				.sin6_flowinfo = 0,
				.sin6_addr = std::move(sk_addr),
				.sin6_scope_id = 0,
			};

			result = *reinterpret_cast<const ::SOCKADDR_STORAGE*>(std::addressof(ipv6_addr));
		}
		break;

		case iconer::net::IpAddressFamily::Unknown:
		{}
		break;
	}

	return result;
}

[[nodiscard]]
::SOCKADDR_STORAGE
SerializeEndpoint(iconer::net::EndPoint&& endpoint)
noexcept
{
	::SOCKADDR_STORAGE result{};
	::SOCKADDR_STORAGE* sockaddr_ptr = std::addressof(result);

	switch (std::move(endpoint).AddressFamily())
	{
		case iconer::net::IpAddressFamily::IPv4:
		{
			::IN_ADDR sk_addr{};
			if (not TrySerializeIpAddress(std::move(endpoint).IpAddress(), std::addressof(sk_addr.s_addr)))
			{
				break;
			}

			::SOCKADDR_IN ipv4_addr
			{
				.sin_family = (std::uint16_t)iconer::net::IpAddressFamily::IPv4,
				.sin_port = std::move(endpoint).Port(),
				.sin_addr = std::move(sk_addr),
			};

			result = *reinterpret_cast<const ::SOCKADDR_STORAGE*>(std::addressof(ipv4_addr));
		}
		break;

		case iconer::net::IpAddressFamily::IPv6:
		{
			IN6_ADDR sk_addr{};
			if (not TrySerializeIpAddress(std::move(endpoint).IpAddress(), std::addressof(sk_addr.s6_addr)))
			{
				break;
			}

			SOCKADDR_IN6 ipv6_addr
			{
				.sin6_family = (std::uint16_t)iconer::net::IpAddressFamily::IPv6,
				.sin6_port = std::move(endpoint).Port(),
				.sin6_flowinfo = 0,
				.sin6_addr = std::move(sk_addr),
				.sin6_scope_id = 0,
			};

			result = *reinterpret_cast<const SOCKADDR_STORAGE*>(std::addressof(ipv6_addr));
		}
		break;

		case iconer::net::IpAddressFamily::Unknown:
		{}
		break;
	}

	return result;
}

iconer::net::Socket::ActionResult
RawGetOption(const iconer::net::Socket::HandleType& sock, int option)
noexcept
{
	int result = 0;
	int len = sizeof(int);

	if (0 == ::getsockopt(sock
		, SOL_SOCKET
		, option
		, reinterpret_cast<char*>(std::addressof(result)), std::addressof(len)))
	{
		return std::nullopt;
	}

	return iconer::net::AcquireNetworkError();
}
