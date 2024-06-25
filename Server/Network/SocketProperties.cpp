module;
#include <WinSock2.h>
#include <WS2tcpip.h>

#define LIKELY [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Net.Socket;
import <print>;

bool
iconer::net::Socket::ReusableAddress()
const noexcept
{
	int opt_val{};
	int opt_len = sizeof(opt_val);

	if (0 == ::getsockopt(myHandle
		, SOL_SOCKET
		, std::to_underlying(SocketOption::Recyclable)
		, reinterpret_cast<char*>(&opt_val), &opt_len)) LIKELY
	{
		return opt_val != 0;
	}

	return false;
}

iconer::net::Socket::IoResult
iconer::net::Socket::ReusableAddress(bool flag)
noexcept
{
	::BOOL iflag = static_cast<::BOOL>(flag);
	reuseAddress.myValue = flag;

	return SetOption(SocketOption::Recyclable, std::addressof(iflag), sizeof(iflag));
}

bool
iconer::net::Socket::IsTcpNoDelay()
const noexcept
{
	int opt_val{};
	int opt_len = sizeof(opt_val);

	if (0 == ::getsockopt(myHandle
		, ::IPPROTO::IPPROTO_TCP
		, std::to_underlying(SocketOption::NoDelay)
		, reinterpret_cast<char*>(&opt_val), &opt_len)) LIKELY
	{
		return opt_val == 0;
	}

	return true;
}

iconer::net::Socket::IoResult
iconer::net::Socket::SetTcpNoDelay(bool flag)
noexcept
{
	::BOOL iflag = static_cast<::BOOL>(!flag);

	if (0 == ::setsockopt(myHandle
		, ::IPPROTO::IPPROTO_TCP
		, std::to_underlying(SocketOption::NoDelay)
		, reinterpret_cast<const char*>(std::addressof(iflag)), sizeof(iflag))) UNLIKELY
	{
		tcpNoDelay.myValue = flag;

		return {};
	}
	else LIKELY
	{
		return std::unexpected{ AcquireNetworkError() };
	}
}

void
iconer::net::Socket::ReuseAddressImplementation(bool flag)
noexcept
{
	std::println("Socket::ReuseAddressImplementation({}) called", flag);

	(void)ReusableAddress(flag);
}

void
iconer::net::Socket::TcpNoDelayImplementation(bool flag)
noexcept
{
	std::println("Socket::TcpNoDelayImplementation({}) called", flag);

	(void)SetTcpNoDelay(flag);
}
