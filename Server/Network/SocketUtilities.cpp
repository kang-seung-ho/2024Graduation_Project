module;
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ws2ipdef.h>
#include <MSWSock.h>

#define LIKELY [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Net.Socket;

constinit static inline ::RIO_EXTENSION_FUNCTION_TABLE rioFunctions{};

iconer::util::Expected<iconer::net::Socket, iconer::net::ErrorCode>
iconer::net::Socket::Create(iconer::net::SocketCategory type
	, iconer::net::InternetProtocol protocol
	, iconer::net::IpAddressFamily family)
	noexcept
{
	const auto flags = std::to_underlying(type);

	std::uintptr_t native{ INVALID_SOCKET };
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

	std::uintptr_t native{ INVALID_SOCKET };
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

	if (INVALID_SOCKET == native) UNLIKELY
	{
		outpin = AcquireNetworkError();

		return false;
	}
	else LIKELY
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
	if (auto result = Socket::Create(type, InternetProtocol::TCP, family); result) LIKELY
	{
		return std::move(result.value());
	}
	else UNLIKELY
	{
		return Socket{};
	}
}

iconer::net::Socket
iconer::net::Socket::CreateUdpSocket(SocketCategory type
	, IpAddressFamily family)
	noexcept
{
	if (auto result = Socket::Create(type, InternetProtocol::UDP, family); result) LIKELY
	{
		return std::move(result.value());
	}
	else UNLIKELY
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
