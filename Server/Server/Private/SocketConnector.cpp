module;
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ws2ipdef.h>

module Iconer.Net.Socket;
import Iconer.Net.EndPoint;
import <coroutine>;

using namespace iconer;

net::Socket::ActionResult
net::Socket::Connect(const net::IpAddress& address, std::uint16_t port)
const noexcept
{
	return this->Connect(EndPoint{ address, port });
}

net::Socket::ActionResult
net::Socket::Connect(net::IpAddress&& address, std::uint16_t port)
const noexcept
{
	return this->Connect(EndPoint{ std::move(address), port });
}

net::Socket::ActionTask
net::Socket::ConnectAsync(const net::EndPoint& endpoint)
const noexcept
{
	co_return this->Connect(endpoint);
}

net::Socket::ActionTask
net::Socket::ConnectAsync(net::EndPoint&& endpoint)
const noexcept
{
	co_return this->Connect(std::move(endpoint));
}

net::Socket::ActionTask
net::Socket::ConnectAsync(const net::IpAddress& address, std::uint16_t port)
const noexcept
{
	co_return this->Connect(EndPoint{ address, port });
}

net::Socket::ActionTask
net::Socket::ConnectAsync(net::IpAddress&& address, std::uint16_t port)
const noexcept
{
	co_return this->Connect(EndPoint{ std::move(address), port });
}

net::Socket::FactoryResult
net::Socket::Accept()
const noexcept
{
	::SOCKADDR_STORAGE address{};
	int address_blen = sizeof(SOCKADDR_STORAGE);

	SOCKADDR* rawaddr = reinterpret_cast<::SOCKADDR*>(std::addressof(address));

	HandleType client = ::accept(myHandle, rawaddr, std::addressof(address_blen));
	if (INVALID_SOCKET == client)
	{
		return std::unexpected{ AcquireNetworkError() };
	}

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
		}
		break;
	}

	return Socket{ client, myProtocol, family };
}

net::Socket::FactoryResult
net::Socket::Accept(EndPoint& endpoint)
const noexcept
{
	::SOCKADDR_STORAGE address{};
	int address_blen = sizeof(SOCKADDR_STORAGE);

	SOCKADDR* rawaddr = reinterpret_cast<::SOCKADDR*>(std::addressof(address));

	HandleType client = ::WSAAccept(myHandle, rawaddr, std::addressof(address_blen), nullptr, 0);
	if (INVALID_SOCKET == client)
	{
		return std::unexpected{ AcquireNetworkError() };
	}

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

	return Socket{ client, myProtocol, family };
}
