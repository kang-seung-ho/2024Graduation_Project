module;
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ws2ipdef.h>
#include <MSWSock.h>

#define LIKELY [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Net.EndPoint;
import <cstring>;

bool TrySerializeIpAddress(const iconer::net::IpAddress& ip_address, void* const& out) noexcept;

iconer::net::SerializedSocketAddress
iconer::net::EndPoint::Serialize()
const noexcept
{
	const auto& ip = GetIpAddress();
	const auto& port = GetPort();

	SerializedSocketAddress result{};
	auto result_ptr = std::addressof(result);

	switch (GetAddressFamily())
	{
	case iconer::net::IpAddressFamily::IPv4:
	{
		::IN_ADDR sk_addr{};

		// s_addr
		if (not TrySerializeIpAddress(ip, std::addressof(sk_addr.S_un.S_addr))) UNLIKELY
		{
			break;
		}

		::SOCKADDR_IN ipv4_addr
		{
			.sin_family = AF_INET,
			.sin_port = port,
			.sin_addr = std::move(sk_addr),
		};

		std::memcpy(result_ptr, std::addressof(ipv4_addr), sizeof(ipv4_addr));
	}
	break;

	case iconer::net::IpAddressFamily::IPv6:
	{
		::IN6_ADDR sk_addr{};

		// s6_addr
		if (not TrySerializeIpAddress(ip, std::addressof(sk_addr.u.Byte))) UNLIKELY
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

		std::memcpy(result_ptr, std::addressof(ipv6_addr), sizeof(ipv6_addr));
	}
	break;

	case iconer::net::IpAddressFamily::Unknown:
	{}
	break;
	}

	return std::move(result);
}

bool
TrySerializeIpAddress(const iconer::net::IpAddress& ip_address, void* const& out)
noexcept
{
	if (1 != ::inet_pton((int)ip_address.GetFamily()
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
