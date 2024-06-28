module;
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ws2ipdef.h>
#include <MSWSock.h>

#define LIKELY [[likely]]
#define UNLIKELY [[unlikely]]
#undef WSANO_DATA

module Iconer.Net.Socket;
import Iconer.Net.IpAddress;
import Iconer.Net.EndPoint;
import Iconer.Net.IoContext;
import <mutex>;

namespace
{
	inline constinit std::once_flag internalInitFlag{};
	inline constexpr unsigned long DEFAULT_ACCEPT_SIZE = sizeof(::sockaddr_in) + 16UL;

	inline constexpr unsigned long closeFlags = TF_DISCONNECT | TF_REUSE_SOCKET;
}

iconer::net::Socket::Socket(std::uintptr_t sock, iconer::net::InternetProtocol protocol, iconer::net::IpAddressFamily family)
noexcept
	: myHandle(sock)
	, myFamily(family), myProtocol(protocol)
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
	switch (myFamily)
	{
	case IpAddressFamily::IPv4:
	{
		const auto sockaddr = endpoint.Serialize();
		const auto sockaddr_ptr = std::addressof(sockaddr);
		const auto address = reinterpret_cast<const ::SOCKADDR*>(sockaddr_ptr);

		if (0 == ::bind(myHandle, address, sizeof(::SOCKADDR_IN))) LIKELY
		{
			return {};
		}
		else UNLIKELY
		{
			return std::unexpected{ AcquireNetworkError() };
		}
	}
	break;

	case IpAddressFamily::IPv6:
	{
		const auto sockaddr = endpoint.Serialize();
		const auto sockaddr_ptr = std::addressof(sockaddr);
		const auto address = reinterpret_cast<const ::SOCKADDR*>(sockaddr_ptr);

		if (0 == ::bind(myHandle, address, sizeof(::SOCKADDR_IN6))) LIKELY
		{
			return {};
		}
		else UNLIKELY
		{
			return std::unexpected{ AcquireNetworkError() };
		}
	}
	break;
	}

	return std::unexpected{ AcquireNetworkError() };
}

iconer::net::Socket::IoResult
iconer::net::Socket::Bind(iconer::net::EndPoint&& endpoint)
const noexcept
{
	switch (myFamily)
	{
	case IpAddressFamily::IPv4:
	{
		const auto sockaddr = endpoint.Serialize();
		const auto sockaddr_ptr = std::addressof(sockaddr);
		const auto address = reinterpret_cast<const ::SOCKADDR*>(sockaddr_ptr);

		if (0 == ::bind(myHandle, address, sizeof(::SOCKADDR_IN))) LIKELY
		{
			return {};
		}
		else UNLIKELY
		{
			return std::unexpected{ AcquireNetworkError() };
		}
	}
	break;

	case IpAddressFamily::IPv6:
	{
		const auto sockaddr = endpoint.Serialize();
		const auto sockaddr_ptr = std::addressof(sockaddr);
		const auto address = reinterpret_cast<const ::SOCKADDR*>(sockaddr_ptr);

		if (0 == ::bind(myHandle, address, sizeof(::SOCKADDR_IN6))) LIKELY
		{
			return {};
		}
		else UNLIKELY
		{
			return std::unexpected{ AcquireNetworkError() };
		}
	}
	break;
	}

	return std::unexpected{ AcquireNetworkError() };
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
	const auto sockaddr = endpoint.Serialize();
	const auto ptr = std::addressof(sockaddr);

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
	const auto sockaddr = endpoint.Serialize();
	const auto ptr = std::addressof(sockaddr);

	if (0 != ::WSAConnect(myHandle
		, reinterpret_cast<const SOCKADDR*>(ptr), sizeof(sockaddr)
		, nullptr, nullptr
		, nullptr, nullptr)) UNLIKELY
	{
		if (const auto error_code = AcquireNetworkError(); error_code != ErrorCode::NonBlockedOperation) UNLIKELY
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
	if (INVALID_SOCKET == client) UNLIKELY
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
	IpAddressFamily family{ IpAddressFamily::Unknown };
	std::uint16_t port{};

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

		if (SOCKET_ERROR == WSAAddressToString(rawaddr, sizeof(address), nullptr, ip_wbuffer, std::addressof(wblen))) UNLIKELY
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
	if (INVALID_SOCKET == client) UNLIKELY
	{
		return std::unexpected{ AcquireNetworkError() };
	}

	return Socket{ client, myProtocol, family };
}

iconer::net::Socket::IoResult
iconer::net::Socket::OptainReceiveMemory(iconer::net::IoContext& context, std::span<std::byte> memory)
const
{
	if (memory.size_bytes() == 0 or memory.data() == nullptr)
	{
		return std::unexpected{ ErrorCode::WSANO_DATA };
	}
	else
	{
		::WSABUF buffer
		{
			.len = 0,
			.buf = reinterpret_cast<char*>(memory.data()),
		};

		::DWORD bytes = 0;
		::DWORD flags = 0;

		if (0 != ::WSARecv(myHandle
			, std::addressof(buffer), 1
			, std::addressof(bytes)
			, std::addressof(flags)
			, std::addressof(context)
			, nullptr))
		{
			if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation)
			{
				return std::unexpected{ std::move(error) };
			}
		}
	}

	return {};
}

iconer::net::Socket::IoResult
iconer::net::Socket::OptainReceiveMemory(iconer::net::IoContext& context, std::byte* memory, size_t size)
const
{
	if (size == 0 or memory == nullptr)
	{
		return std::unexpected{ ErrorCode::WSANO_DATA };
	}
	else
	{
		::WSABUF buffer
		{
			.len = 0,
			.buf = reinterpret_cast<char*>(memory),
		};

		::DWORD bytes = 0;
		::DWORD flags = 0;

		if (0 != ::WSARecv(myHandle
			, std::addressof(buffer), 1
			, std::addressof(bytes)
			, std::addressof(flags)
			, std::addressof(context)
			, nullptr))
		{
			if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation)
			{
				return std::unexpected{ std::move(error) };
			}
		}
	}

	return {};
}

iconer::net::Socket::IoResult
iconer::net::Socket::OptainReceiveMemory(iconer::net::IoContext* context, std::span<std::byte> memory)
const
{
	if (memory.size_bytes() == 0 or memory.data() == nullptr)
	{
		return std::unexpected{ ErrorCode::WSANO_DATA };
	}
	else
	{
		::WSABUF buffer
		{
			.len = 0,
			.buf = reinterpret_cast<char*>(memory.data()),
		};

		::DWORD bytes = 0;
		::DWORD flags = 0;

		if (0 != ::WSARecv(myHandle
			, std::addressof(buffer), 1
			, std::addressof(bytes)
			, std::addressof(flags)
			, context
			, nullptr))
		{
			if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation)
			{
				return std::unexpected{ std::move(error) };
			}
		}
	}

	return {};
}

iconer::net::Socket::IoResult
iconer::net::Socket::OptainReceiveMemory(iconer::net::IoContext* context, std::byte* memory, size_t size)
const
{
	if (size == 0 or memory == nullptr)
	{
		return std::unexpected{ ErrorCode::WSANO_DATA };
	}
	else
	{
		::WSABUF buffer
		{
			.len = 0,
			.buf = reinterpret_cast<char*>(memory),
		};

		::DWORD bytes = 0;
		::DWORD flags = 0;

		if (0 != ::WSARecv(myHandle
			, std::addressof(buffer), 1
			, std::addressof(bytes)
			, std::addressof(flags)
			, context
			, nullptr))
		{
			if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation)
			{
				return std::unexpected{ std::move(error) };
			}
		}
	}

	return {};
}

iconer::net::Socket::IoResult
iconer::net::Socket::BeginAccept(iconer::net::IoContext& context, const iconer::net::Socket& client)
const
{
	return BeginAccept(std::addressof(context), client);
}

iconer::net::Socket::IoResult
iconer::net::Socket::BeginAccept(iconer::net::IoContext& context, const Socket& client, std::span<std::byte> accept_buffer)
const
{
	return BeginAccept(std::addressof(context), client, std::move(accept_buffer));
}

iconer::net::Socket::IoResult
iconer::net::Socket::BeginAccept(iconer::net::IoContext* context, const iconer::net::Socket& client)
const
{
	if (context == nullptr or myHandle == invalidHandle or client.myHandle == invalidHandle) UNLIKELY
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
		if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation) UNLIKELY
		{
			return std::unexpected{ std::move(error) };
		}
	}

	return {};
}

iconer::net::Socket::IoResult
iconer::net::Socket::BeginAccept(iconer::net::IoContext* context, const iconer::net::Socket& client, std::span<std::byte> accept_buffer)
const
{
	if (context == nullptr or myHandle == invalidHandle or client.myHandle == invalidHandle) UNLIKELY
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
		if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation) UNLIKELY
		{
			return std::unexpected{ std::move(error) };
		}
	}

	return {};
}

iconer::net::Socket::IoResult
iconer::net::Socket::EndAccept(const Socket& listener)
const noexcept
{
	return SetOption(SocketOption::UpdateContext, std::addressof(listener.myHandle), sizeof(myHandle));
}

iconer::net::Socket::IoResult
iconer::net::Socket::Close()
const noexcept
{
	if (0 != asyncTransmitFnPtr(myHandle, nullptr, 0, 0, nullptr, nullptr, closeFlags)) LIKELY
	{
		return {};
	}
	else if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation) UNLIKELY
	{
		return std::unexpected{ std::move(error) };
	}
	else LIKELY
	{
		return {};
	}
}

bool
iconer::net::Socket::Close(ErrorCode& error_code)
const noexcept
{
	if (myHandle != invalidHandle)
	{
		if (0 != asyncTransmitFnPtr(myHandle, nullptr, 0, 0, nullptr, nullptr, closeFlags)) LIKELY
		{
			return true;
		}
		else UNLIKELY
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

iconer::net::Socket::IoResult
iconer::net::Socket::AsyncClose(IoContext& context)
const noexcept
{
	return AsyncClose(std::addressof(context));
}

iconer::net::Socket::IoResult
iconer::net::Socket::AsyncClose(IoContext* const context)
const noexcept
{
	if (0 != asyncTransmitFnPtr(myHandle, nullptr, 0, 0, context, nullptr, closeFlags)) LIKELY
	{
		return {};
	}
	else if (auto error = AcquireNetworkError(); error != ErrorCode::PendedIoOperation) UNLIKELY
	{
		return std::unexpected{ std::move(error) };
	}
	else LIKELY
	{
		return {};
	}
}

bool
iconer::net::Socket::AsyncClose(IoContext& context, ErrorCode& error_code)
const noexcept
{
	return AsyncClose(std::addressof(context), error_code);
}

bool
iconer::net::Socket::AsyncClose(IoContext* const context, ErrorCode& error_code)
const noexcept
{
	if (myHandle != invalidHandle) LIKELY
	{
		if (0 != asyncTransmitFnPtr(myHandle, nullptr, 0, 0, context, nullptr, closeFlags)) LIKELY
		{
			return true;
		}
		else UNLIKELY
		{
			error_code = AcquireNetworkError();

			return false;
		}
	}
	else UNLIKELY
	{
		error_code = ErrorCode::NotASocket;
		return false;
	}
}
