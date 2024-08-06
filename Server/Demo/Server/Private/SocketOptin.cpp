module;
#include <WinSock2.h>
#include <ws2ipdef.h>

module Iconer.Net.Socket;
import Iconer.Utility.Byte;
import Iconer.Net.IpAddress;
import Iconer.Net.EndPoint;
import <type_traits>;
import <mutex>;

using namespace iconer;

net::Socket::ActionResult
net::Socket::Bind(const net::IpAddress& address, std::uint16_t port)
const noexcept
{
	return this->Bind(EndPoint{ address, port });
}

net::Socket::ActionResult
net::Socket::Bind(net::IpAddress&& address, std::uint16_t port)
const noexcept
{
	return this->Bind(EndPoint{ std::move(address), port });
}

iconer::net::Socket::ActionResult
iconer::net::Socket::BindAny(std::uint16_t port)
const noexcept
{
	switch (myFamily)
	{
		case IpAddressFamily::IPv4:
		{
			::SOCKADDR_IN ipv4_sockaddr
			{
				.sin_family = AF_INET,
				.sin_port = iconer::os::ToNetworkByteOrder(port),
				.sin_addr = ::in4addr_any,
				.sin_zero{}
			};

			if (0 == ::bind(myHandle, reinterpret_cast<const SOCKADDR*>(std::addressof(ipv4_sockaddr)), sizeof(ipv4_sockaddr)))
			{
				return std::nullopt;
			}
		}
		break;

		case IpAddressFamily::IPv6:
		{
			::SOCKADDR_IN6 ipv6_sockaddr
			{
				.sin6_family = AF_INET6,
				.sin6_port = iconer::os::ToNetworkByteOrder(port),
				.sin6_flowinfo = 0,
				.sin6_addr = ::in6addr_any
			};

			if (0 == ::bind(myHandle, reinterpret_cast<const ::SOCKADDR*>(std::addressof(ipv6_sockaddr)), sizeof(ipv6_sockaddr)))
			{
				return std::nullopt;
			}
		}
		break;
	}

	return AcquireNetworkError();
}

net::Socket::ActionResult
net::Socket::BindHost(std::uint16_t port)
const noexcept
{
	switch (myFamily)
	{
		case IpAddressFamily::IPv4:
		{
			::SOCKADDR_IN ipv4_sockaddr
			{
				.sin_family = AF_INET,
				.sin_port = iconer::os::ToNetworkByteOrder(port),
				.sin_addr = ::in4addr_loopback,
				.sin_zero{}
			};

			if (0 == ::bind(myHandle, reinterpret_cast<const SOCKADDR*>(std::addressof(ipv4_sockaddr)), sizeof(ipv4_sockaddr)))
			{
				return std::nullopt;
			}
		}
		break;

		case IpAddressFamily::IPv6:
		{
			::SOCKADDR_IN6 ipv6_sockaddr
			{
				.sin6_family = AF_INET6,
				.sin6_port = iconer::os::ToNetworkByteOrder(port),
				.sin6_flowinfo = 0,
				.sin6_addr = ::in6addr_loopback
			};

			if (0 == ::bind(myHandle, reinterpret_cast<const ::SOCKADDR*>(std::addressof(ipv6_sockaddr)), sizeof(ipv6_sockaddr)))
			{
				return std::nullopt;
			}
		}
		break;
	}

	return AcquireNetworkError();
}

iconer::net::Socket::ActionResult
iconer::net::Socket::ConnectToAny(std::uint16_t port)
const noexcept
{
	switch (myFamily)
	{
		case IpAddressFamily::IPv4:
		{
			::SOCKADDR_IN ipv4_sockaddr
			{
				.sin_family = AF_INET,
				.sin_port = iconer::os::ToNetworkByteOrder(port),
				.sin_addr = ::in4addr_any,
				.sin_zero{}
			};

			if (0 != ::WSAConnect(myHandle
				, reinterpret_cast<const SOCKADDR*>(std::addressof(ipv4_sockaddr)), sizeof(ipv4_sockaddr)
				, nullptr, nullptr
				, nullptr, nullptr))
			{
				if (auto error_code = AcquireNetworkError(); error_code != ErrorCode::NonBlockedOperation)
				{
					return error_code;
				}
				else
				{
					return std::nullopt;
				}
			}
		}
		break;

		case IpAddressFamily::IPv6:
		{
			::SOCKADDR_IN6 ipv6_sockaddr
			{
				.sin6_family = AF_INET6,
				.sin6_port = iconer::os::ToNetworkByteOrder(port),
				.sin6_flowinfo = 0,
				.sin6_addr = ::in6addr_any
			};

			if (0 != ::WSAConnect(myHandle
				, reinterpret_cast<const SOCKADDR*>(std::addressof(ipv6_sockaddr)), sizeof(ipv6_sockaddr)
				, nullptr, nullptr
				, nullptr, nullptr))
			{
				if (auto error_code = AcquireNetworkError(); error_code != ErrorCode::NonBlockedOperation)
				{
					return error_code;
				}
				else
				{
					return std::nullopt;
				}
			}
		}
		break;
	}

	return std::nullopt;
}

iconer::net::Socket::ActionResult
iconer::net::Socket::ConnectToHost(std::uint16_t port)
const noexcept
{
	switch (myFamily)
	{
		case IpAddressFamily::IPv4:
		{
			::SOCKADDR_IN sockaddr
			{
				.sin_family = AF_INET,
				.sin_port = iconer::os::ToNetworkByteOrder(port),
				.sin_addr = ::in4addr_loopback,
				.sin_zero{}
			};

			if (0 != ::WSAConnect(myHandle
				, reinterpret_cast<const SOCKADDR*>(std::addressof(sockaddr)), sizeof(sockaddr)
				, nullptr, nullptr
				, nullptr, nullptr))
			{
				if (auto error_code = AcquireNetworkError(); error_code != ErrorCode::NonBlockedOperation)
				{
					return error_code;
				}
				else
				{
					return std::nullopt;
				}
			}
		}
		break;

		case IpAddressFamily::IPv6:
		{
			::SOCKADDR_IN6 sockaddr
			{
				.sin6_family = AF_INET6,
				.sin6_port = iconer::os::ToNetworkByteOrder(port),
				.sin6_flowinfo = 0,
				.sin6_addr = ::in6addr_loopback
			};

			if (0 != ::WSAConnect(myHandle
				, reinterpret_cast<const SOCKADDR*>(std::addressof(sockaddr)), sizeof(sockaddr)
				, nullptr, nullptr
				, nullptr, nullptr))
			{
				if (auto error_code = AcquireNetworkError(); error_code != ErrorCode::NonBlockedOperation)
				{
					return error_code;
				}
				else
				{
					return std::nullopt;
				}
			}
		}
		break;
	}

	return std::nullopt;
}

iconer::net::Socket::ActionResult
RawSetOption(iconer::net::Socket::HandleType handle, const iconer::net::SocketOptions& option, const void* const& opt_buffer, const size_t& opt_size)
noexcept
{
	if (0 == ::setsockopt(handle
		, SOL_SOCKET
		, std::to_underlying(option)
		, reinterpret_cast<const char*>(opt_buffer), static_cast<int>(opt_size)))
	{
		return std::nullopt;
	}

	return iconer::net::AcquireNetworkError();
}

iconer::net::Socket::ActionResult
iconer::net::Socket::SetOption(SocketOptions option, const void* opt_buffer, const size_t opt_size)
const noexcept
{
	return RawSetOption(Super::GetHandle(), option, opt_buffer, opt_size);
}

bool
net::Socket::ReusableAddress()
const noexcept
{
	return IsAddressReusable;
}

void
net::Socket::ReusableAddress(bool flag)
noexcept
{
	IsAddressReusable = flag;
}

iconer::net::Socket::ActionResult
iconer::net::Socket::DelegateAddressReusable::operator()(const bool& flag)
noexcept
{
	::BOOL iflag = static_cast<::BOOL>(flag);

	return RawSetOption(handle, SocketOptions::Recyclable, std::addressof(iflag), sizeof(iflag));
}
