module;
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ws2ipdef.h>
#include <MSWSock.h>

#define LIKELY [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Net.Socket;
import Iconer.Net.IpAddress;
import Iconer.Net.EndPoint;
import Iconer.Net.IoContext;
import <mutex>;

::SOCKADDR_STORAGE SerializeEndpoint(const iconer::net::EndPoint& endpoint) noexcept;

constinit static inline ::RIO_EXTENSION_FUNCTION_TABLE rioFunctions{};
constinit static inline std::once_flag internalInitFlag{};
static inline constexpr unsigned long DEFAULT_ACCEPT_SIZE = sizeof(::sockaddr_in) + 16UL;

iconer::net::Socket::Socket(std::uintptr_t sock, iconer::net::InternetProtocol protocol, iconer::net::IpAddressFamily family)
noexcept
	: myHandle(sock)
	, myProtocol(protocol), myFamily(family)
{
	std::call_once(internalInitFlag, &Socket::InternalFunctionInitializer, sock);
}

iconer::net::Socket::IoResult
iconer::net::Socket::Bind(const iconer::net::IpAddress& address, std::uint16_t port)
const noexcept
{
	return Bind(iconer::net::EndPoint{ address, port });
}

iconer::net::Socket::IoResult
iconer::net::Socket::Bind(iconer::net::IpAddress&& address, std::uint16_t port)
const noexcept
{
	return Bind(iconer::net::EndPoint{ std::move(address), port });
}

iconer::net::Socket::IoResult
iconer::net::Socket::Bind(const iconer::net::EndPoint& endpoint)
const noexcept
{
	const auto sockaddr_storage = ::SerializeEndpoint(endpoint);
	const auto ptr = std::addressof(sockaddr_storage);
	const auto address = reinterpret_cast<const ::SOCKADDR*>(ptr);

	if (0 == ::bind(myHandle, address, sizeof(::sockaddr_in))) LIKELY
	{
		return {};
	}
	else UNLIKELY
	{
		return std::unexpected{ AcquireNetworkError() };
	}
}

iconer::net::Socket::IoResult
iconer::net::Socket::Bind(iconer::net::EndPoint&& endpoint)
const noexcept
{
	const auto sockaddr_storage = ::SerializeEndpoint(std::move(endpoint));
	const auto ptr = std::addressof(sockaddr_storage);
	const auto address = reinterpret_cast<const ::SOCKADDR*>(ptr);

	if (0 == ::bind(myHandle, reinterpret_cast<const ::SOCKADDR*>(ptr), sizeof(sockaddr_in))) LIKELY
	{
		return {};
	}
	else UNLIKELY
	{
		return std::unexpected{ AcquireNetworkError() };
	}
}

iconer::net::Socket::IoResult
iconer::net::Socket::BindToAny(std::uint16_t port)
const noexcept
{
	switch (myFamily)
	{
	case IpAddressFamily::IPv4:
	{
		::SOCKADDR_IN ipv4_sockaddr
		{
			.sin_family = AF_INET,
			.sin_port = ::htons(port),
			.sin_addr = ::in4addr_any,
			.sin_zero{}
		};

		const auto address = reinterpret_cast<const SOCKADDR*>(std::addressof(ipv4_sockaddr));

		if (0 == ::bind(myHandle, address, sizeof(ipv4_sockaddr))) LIKELY
		{
			return {};
		}
	}
	break;

	case IpAddressFamily::IPv6:
	{
		::SOCKADDR_IN6 ipv6_sockaddr
		{
			.sin6_family = AF_INET6,
			.sin6_port = ::htons(port),
			.sin6_flowinfo = 0,
			.sin6_addr = ::in6addr_any
		};

		const auto address = reinterpret_cast<const SOCKADDR*>(std::addressof(ipv6_sockaddr));

		if (0 == ::bind(myHandle, address, sizeof(ipv6_sockaddr))) LIKELY
		{
			return {};
		}
	}
	break;
	}

	return std::unexpected{ AcquireNetworkError() };
}

iconer::net::Socket::IoResult
iconer::net::Socket::BindToHost(std::uint16_t port)
const noexcept
{
	switch (myFamily)
	{
	case IpAddressFamily::IPv4:
	{
		::SOCKADDR_IN ipv4_sockaddr
		{
			.sin_family = AF_INET,
			.sin_port = ::htons(port),
			.sin_addr = ::in4addr_loopback,
			.sin_zero{}
		};

		const auto address = reinterpret_cast<const SOCKADDR*>(std::addressof(ipv4_sockaddr));

		if (0 == ::bind(myHandle, address, sizeof(ipv4_sockaddr))) LIKELY
		{
			return {};
		}
	}
	break;

	case IpAddressFamily::IPv6:
	{
		::SOCKADDR_IN6 ipv6_sockaddr
		{
			.sin6_family = AF_INET6,
			.sin6_port = ::htons(port),
			.sin6_flowinfo = 0,
			.sin6_addr = ::in6addr_loopback
		};

		const auto address = reinterpret_cast<const SOCKADDR*>(std::addressof(ipv6_sockaddr));

		if (0 == ::bind(myHandle, address, sizeof(ipv6_sockaddr))) LIKELY
		{
			return {};
		}
	}
	break;
	}

	return std::unexpected{ AcquireNetworkError() };
}

iconer::net::Socket::IoResult
iconer::net::Socket::Open()
const noexcept
{
	if (const int open = ::listen(myHandle, SOMAXCONN); SOCKET_ERROR == open) UNLIKELY
	{
		return std::unexpected{ AcquireNetworkError() };
	}
	else LIKELY
	{
		return {};
	}
}

iconer::net::Socket::IoResult
iconer::net::Socket::Connect(const iconer::net::IpAddress& address, std::uint16_t port)
const noexcept
{
	return Connect(EndPoint{ address, port });
}

iconer::net::Socket::IoResult
iconer::net::Socket::Connect(iconer::net::IpAddress&& address, std::uint16_t port)
const noexcept
{
	return Connect(EndPoint{ std::move(address), port });
}

iconer::net::Socket::IoResult
iconer::net::Socket::Connect(const iconer::net::EndPoint& endpoint)
const noexcept
{
	const SOCKADDR_STORAGE sockaddr = SerializeEndpoint(endpoint);
	const SOCKADDR_STORAGE* ptr = std::addressof(sockaddr);

	if (0 != ::WSAConnect(myHandle
		, reinterpret_cast<const SOCKADDR*>(ptr), sizeof(sockaddr)
		, nullptr, nullptr
		, nullptr, nullptr))
	{
		if (const auto error_code = AcquireNetworkError(); error_code != ErrorCode::NonBlockedOperation) UNLIKELY
		{
			return std::unexpected{ AcquireNetworkError() };
		}
	}

	return {};
}

iconer::net::Socket::IoResult
iconer::net::Socket::Connect(iconer::net::EndPoint&& endpoint)
const noexcept
{
	const SOCKADDR_STORAGE sockaddr = SerializeEndpoint(std::move(endpoint));
	const SOCKADDR_STORAGE* ptr = std::addressof(sockaddr);

	if (0 != ::WSAConnect(myHandle
		, reinterpret_cast<const SOCKADDR*>(ptr), sizeof(sockaddr)
		, nullptr, nullptr
		, nullptr, nullptr)) UNLIKELY
	{
		if (const auto error_code = AcquireNetworkError(); error_code != ErrorCode::NonBlockedOperation)
		{
			return std::unexpected{ AcquireNetworkError() };
		}
	}

	return {};
}

iconer::net::Socket::IoResult
iconer::net::Socket::ConnectToHost(std::uint16_t port)
const noexcept
{
	switch (myFamily)
	{
	case IpAddressFamily::IPv4:
	{
		const ::SOCKADDR_IN sockaddr
		{
			.sin_family = AF_INET,
			.sin_port = ::htons(port),
			.sin_addr = ::in4addr_loopback,
			.sin_zero{}
		};

		if (0 != ::WSAConnect(myHandle
			, reinterpret_cast<const SOCKADDR*>(std::addressof(sockaddr)), sizeof(sockaddr)
			, nullptr, nullptr
			, nullptr, nullptr)) UNLIKELY
		{
			if (auto error_code = AcquireNetworkError(); error_code != ErrorCode::NonBlockedOperation) UNLIKELY
			{
				return std::unexpected{ AcquireNetworkError() };
			}
		}

		return {};
	}
	break;

	case IpAddressFamily::IPv6:
	{
		const ::SOCKADDR_IN6 sockaddr
		{
			.sin6_family = AF_INET6,
			.sin6_port = ::htons(port),
			.sin6_flowinfo = 0,
			.sin6_addr = ::in6addr_loopback
		};

		if (0 != ::WSAConnect(myHandle
			, reinterpret_cast<const SOCKADDR*>(std::addressof(sockaddr)), sizeof(sockaddr)
			, nullptr, nullptr
			, nullptr, nullptr)) UNLIKELY
		{
			if (auto error_code = AcquireNetworkError(); error_code != ErrorCode::NonBlockedOperation) UNLIKELY
			{
				return std::unexpected{ AcquireNetworkError() };
			}
		}

		return {};
	}
	break;

	default:
	{
		return std::unexpected{ AcquireNetworkError() };
	}
	}
}

iconer::net::Socket::IoResult
iconer::net::Socket::SetOption(SocketOption option, const void* opt_buffer, const size_t opt_size)
const noexcept
{
	if (0 == ::setsockopt(myHandle
		, SOL_SOCKET
		, std::to_underlying(option)
		, reinterpret_cast<const char*>(opt_buffer), static_cast<int>(opt_size))) UNLIKELY
	{
		return {};
	}
	else LIKELY
	{
		return std::unexpected{ AcquireNetworkError() };
	}
}

bool
iconer::net::Socket::ReusableAddress()
const noexcept
{
	int opt_val{};
	int opt_len = sizeof(opt_val);

	if (0 == ::getsockopt(myHandle
		, SOL_SOCKET
		, std::to_underlying(SocketOption::Recyclable)
		, reinterpret_cast<char*>(&opt_val), &opt_len))
	{
		return opt_val != 0;
	}

	return false;
}

iconer::net::Socket::IoResult
iconer::net::Socket::ReusableAddress(bool flag)
const noexcept
{
	::BOOL iflag = static_cast<::BOOL>(flag);

	return SetOption(SocketOption::Recyclable, std::addressof(iflag), sizeof(iflag));
}

std::expected<iconer::net::Socket, iconer::net::ErrorCode>
iconer::net::Socket::Accept()
const noexcept
{
	::SOCKADDR_STORAGE address{};
	int address_blen = sizeof(SOCKADDR_STORAGE);

	SOCKADDR* rawaddr = reinterpret_cast<::SOCKADDR*>(std::addressof(address));

	IpAddressFamily family;
	switch (address.ss_family)
	{
	case AF_INET:
	{
		family = IpAddressFamily::IPv4;
	}
	break;
	case AF_INET6:
	{
		family = IpAddressFamily::IPv6;
	}
	break;
	default:
	{
		family = IpAddressFamily::Unknown;

		return std::unexpected{ AcquireNetworkError() };
	}
	break;
	}

	const auto client = ::accept(myHandle, rawaddr, std::addressof(address_blen));
	if (INVALID_SOCKET == client)
	{
		return std::unexpected{ AcquireNetworkError() };
	}

	return Socket{ client, myProtocol, family };
}

std::expected<iconer::net::Socket, iconer::net::ErrorCode>
iconer::net::Socket::Accept(iconer::net::EndPoint& endpoint)
const noexcept
{
	::SOCKADDR_STORAGE address{};
	int address_blen = sizeof(SOCKADDR_STORAGE);

	SOCKADDR* rawaddr = reinterpret_cast<::SOCKADDR*>(std::addressof(address));

	IpAddress ip{ IpAddressFamily::Unknown, "" };
	IpAddressFamily family;
	std::uint16_t port;

	switch (address.ss_family)
	{
	case AF_INET:
	{
		family = IpAddressFamily::IPv4;

		::SOCKADDR_IN* addr = reinterpret_cast<::SOCKADDR_IN*>(std::addressof(address));
		port = ::ntohs(addr->sin_port);

		try
		{
			char ip_buffer[16]{};

			const char* ip_address = ::inet_ntop(AF_INET, std::addressof(addr->sin_addr), ip_buffer, sizeof(ip_buffer));
			ip = IpAddress{ IpAddressFamily::IPv4, ip_address };
		}
		catch (...)
		{
			return std::unexpected{ AcquireNetworkError() };
		}
	}
	break;

	case AF_INET6:
	{
		family = IpAddressFamily::IPv6;

		::SOCKADDR_IN6* addr = reinterpret_cast<::SOCKADDR_IN6*>(std::addressof(address));
		port = ::ntohs(addr->sin6_port);

		wchar_t ip_wbuffer[32]{};
		::DWORD wblen = sizeof(ip_wbuffer);

		if (SOCKET_ERROR == WSAAddressToString(rawaddr, sizeof(address), nullptr, ip_wbuffer, std::addressof(wblen)))
		{
			return std::unexpected{ AcquireNetworkError() };
		}

		try
		{
			size_t mb_result = 0;
			char ip_buffer[32]{};
			::mbstowcs_s(std::addressof(mb_result), ip_wbuffer, ip_buffer, ::strlen(ip_buffer) + 1);

			ip = IpAddress{ IpAddressFamily::IPv6, std::move(ip_buffer) };
		}
		catch (...)
		{
			return std::unexpected{ AcquireNetworkError() };
		}
	}
	break;

	default:
	{
		family = IpAddressFamily::Unknown;
	}
	break;
	}
	endpoint = EndPoint{ std::move(ip), port };

	const auto client = ::WSAAccept(myHandle, rawaddr, std::addressof(address_blen), nullptr, 0);
	if (INVALID_SOCKET == client)
	{
		return std::unexpected{ AcquireNetworkError() };
	}

	return Socket{ client, myProtocol, family };
}

iconer::net::Socket::IoResult
iconer::net::Socket::BeginAccept(iconer::net::IoContext& context, iconer::net::Socket& client)
const
{
	return BeginAccept(std::addressof(context), client);
}

iconer::net::Socket::IoResult
iconer::net::Socket::BeginAccept(iconer::net::IoContext& context, Socket& client, std::span<std::byte> accept_buffer)
const
{
	return BeginAccept(std::addressof(context), client, std::move(accept_buffer));
}

iconer::net::Socket::IoResult
iconer::net::Socket::BeginAccept(iconer::net::IoContext* context, iconer::net::Socket& client)
const
{
	if (context == nullptr or myHandle == invalidHandle or client.myHandle == invalidHandle)
	{
		return std::unexpected{ ErrorCode::NotASocket };
	}

	char temp_buffer[::DEFAULT_ACCEPT_SIZE * 2]{};
	::DWORD result_bytes{};

	if (0 == asyncAcceptFnPtr(myHandle, client.myHandle
		, temp_buffer, 0UL
		, ::DEFAULT_ACCEPT_SIZE
		, ::DEFAULT_ACCEPT_SIZE
		, std::addressof(result_bytes)
		, static_cast<::LPWSAOVERLAPPED>(context))
		)
	{
		if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation)
		{
			return std::unexpected{ std::move(error) };
		}
	}

	return {};
}

iconer::net::Socket::IoResult
iconer::net::Socket::BeginAccept(iconer::net::IoContext* context, iconer::net::Socket& client, std::span<std::byte> accept_buffer)
const
{
	if (context == nullptr or myHandle == invalidHandle or client.myHandle == invalidHandle)
	{
		return std::unexpected{ ErrorCode::NotASocket };
	}

	::DWORD result_bytes{};

	if (0 == asyncAcceptFnPtr(myHandle, client.myHandle
		, accept_buffer.data(), static_cast<::DWORD>(accept_buffer.size_bytes())
		, ::DEFAULT_ACCEPT_SIZE
		, ::DEFAULT_ACCEPT_SIZE
		, std::addressof(result_bytes)
		, static_cast<::LPWSAOVERLAPPED>(context))
		)
	{
		if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation)
		{
			return std::unexpected{ std::move(error) };
		}
	}

	return {};
}

iconer::net::Socket::IoResult
iconer::net::Socket::EndAccept(Socket& listener)
const noexcept
{
	return SetOption(SocketOption::UpdateContext, std::addressof(listener.myHandle), sizeof(myHandle));
}

bool
iconer::net::Socket::Close()
const noexcept
{
	if (myHandle != invalidHandle)
	{
		return (0 != asyncTransmitFnPtr(myHandle, nullptr, 0, 0, nullptr, nullptr, TF_DISCONNECT | TF_REUSE_SOCKET));
	}
	else
	{
		return false;
	}
}

bool
iconer::net::Socket::Close(ErrorCode& error_code)
const noexcept
{
	if (myHandle != invalidHandle)
	{
		if (0 != asyncTransmitFnPtr(myHandle, nullptr, 0, 0, nullptr, nullptr, TF_DISCONNECT | TF_REUSE_SOCKET))
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

bool
iconer::net::Socket::AsyncClose(IoContext& context)
const noexcept
{
	return AsyncClose(std::addressof(context));
}

bool
iconer::net::Socket::AsyncClose(IoContext& context, ErrorCode& error_code)
const noexcept
{
	return AsyncClose(std::addressof(context), error_code);
}

bool
iconer::net::Socket::AsyncClose(IoContext* const context)
const noexcept
{
	if (myHandle != invalidHandle)
	{
		return (0 != asyncTransmitFnPtr(myHandle, nullptr, 0, 0, context, nullptr, TF_DISCONNECT | TF_REUSE_SOCKET));
	}
	else
	{
		return false;
	}
}

bool
iconer::net::Socket::AsyncClose(IoContext* const context, ErrorCode& error_code)
const noexcept
{
	if (myHandle != invalidHandle)
	{
		if (0 != asyncTransmitFnPtr(myHandle, nullptr, 0, 0, context, nullptr, TF_DISCONNECT | TF_REUSE_SOCKET))
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

std::expected<iconer::net::Socket, iconer::net::ErrorCode>
iconer::net::Socket::Create(iconer::net::SocketCategory type
	, iconer::net::InternetProtocol protocol
	, iconer::net::IpAddressFamily family)
	noexcept
{
	const auto flags = std::to_underlying(type);

	std::uintptr_t native;
	switch (protocol)
	{
	case InternetProtocol::TCP:
	{
		native = ::WSASocket(static_cast<int>(family), SOCK_STREAM, ::IPPROTO::IPPROTO_TCP, nullptr, 0, flags);
	}
	break;

	case InternetProtocol::UDP:
	{
		native = ::WSASocket(static_cast<int>(family), SOCK_DGRAM, ::IPPROTO::IPPROTO_UDP, nullptr, 0, flags);
	}
	break;

	case InternetProtocol::Unknown:
	{
		return std::unexpected{ AcquireNetworkError() };
	}
	}

	if (INVALID_SOCKET == native)
	{
		return std::unexpected{ AcquireNetworkError() };
	}
	else
	{
		return Socket{ native, protocol, family };
	}
}

bool
iconer::net::Socket::TryCreate(iconer::net::SocketCategory type
	, iconer::net::InternetProtocol protocol
	, iconer::net::IpAddressFamily family
	, iconer::net::Socket& result
	, iconer::net::ErrorCode& outpin)
	noexcept
{
	const auto flags = std::to_underlying(type);

	std::uintptr_t native;
	switch (protocol)
	{
	case InternetProtocol::TCP:
	{
		native = ::WSASocket(static_cast<int>(family), SOCK_STREAM, ::IPPROTO::IPPROTO_TCP, nullptr, 0, flags);
	}
	break;

	case InternetProtocol::UDP:
	{
		native = ::WSASocket(static_cast<int>(family), SOCK_DGRAM, ::IPPROTO::IPPROTO_UDP, nullptr, 0, flags);
	}
	break;

	case InternetProtocol::Unknown:
	{
		outpin = AcquireNetworkError();

		return false;
	}
	}

	if (INVALID_SOCKET == native)
	{
		outpin = AcquireNetworkError();

		return false;
	}
	else
	{
		result = Socket{ native, protocol, family };

		return true;
	}
}

iconer::net::Socket
iconer::net::Socket::CreateTcpSocket(iconer::net::SocketCategory type
	, iconer::net::IpAddressFamily family)
	noexcept
{
	if (auto result = Socket::Create(type, InternetProtocol::TCP, family); result)
	{
		return std::move(result.value());
	}
	else
	{
		return Socket{};
	}
}

iconer::net::Socket
iconer::net::Socket::CreateUdpSocket(SocketCategory type
	, IpAddressFamily family)
	noexcept
{
	if (auto result = Socket::Create(type, InternetProtocol::UDP, family); result)
	{
		return std::move(result.value());
	}
	else
	{
		return Socket{};
	}
}

void
iconer::net::Socket::InternalFunctionInitializer(std::uintptr_t socket)
{
	::GUID fntable_id = WSAID_MULTIPLE_RIO;
	::DWORD temp_bytes = 0;

	::GUID* fntable_addr = std::addressof(fntable_id);
	::DWORD* bytes_addr = std::addressof(temp_bytes);

#if _DEBUG
	int result =
#endif // _DEBUG
		::WSAIoctl(socket, SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER
			, fntable_addr, sizeof(GUID)
			, reinterpret_cast<void**>(std::addressof(rioFunctions)), sizeof(rioFunctions)
			, bytes_addr
			, nullptr, nullptr);

	fntable_id = WSAID_ACCEPTEX;
#if _DEBUG
	result =
#endif // _DEBUG
		::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER
			, fntable_addr, sizeof(GUID)
			, std::addressof(asyncAcceptFnPtr), sizeof(asyncAcceptFnPtr)
			, bytes_addr
			, nullptr, nullptr);

	fntable_id = WSAID_TRANSMITFILE;
#if _DEBUG
	result =
#endif // _DEBUG
		::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER
			, fntable_addr, sizeof(GUID)
			, std::addressof(asyncTransmitFnPtr), sizeof(asyncTransmitFnPtr)
			, bytes_addr
			, nullptr, nullptr);
}

bool
TrySerializeIpAddress(const iconer::net::IpAddress& ip_address, void* const& out)
noexcept
{
	if (1 != ::inet_pton((int) ip_address.GetFamily()
		, ip_address.GetAddressString().data()
		, out)) UNLIKELY
	{
		return false;
	}
	else LIKELY
	{
		return true;
	}
}

::SOCKADDR_STORAGE
SerializeEndpoint(const iconer::net::EndPoint& endpoint)
noexcept
{
	const auto& ip = endpoint.GetIpAddress();
	const auto& port = endpoint.GetPort();

	::SOCKADDR_STORAGE result{};
	::SOCKADDR_STORAGE* sockaddr_ptr = std::addressof(result);

	switch (endpoint.GetAddressFamily())
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
			.sin_family = AF_INET,
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
			.sin6_family = AF_INET6,
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
