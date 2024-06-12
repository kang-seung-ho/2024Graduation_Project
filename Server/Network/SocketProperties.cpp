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
const noexcept
{
	::BOOL iflag = static_cast<::BOOL>(flag);

	return SetOption(SocketOption::Recyclable, std::addressof(iflag), sizeof(iflag));
}

void
iconer::net::Socket::ReuseAddressImplementation(bool flag)
const noexcept
{
	std::println("Socket::ReuseAddressImplementation({}) called", flag);

	(void)ReusableAddress(flag);
}
