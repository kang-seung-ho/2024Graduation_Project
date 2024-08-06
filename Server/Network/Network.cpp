module;
#pragma warning(push)
#pragma warning(disable : 4006)
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#include <WinSock2.h>
#pragma warning(pop)

module Iconer.Net;
import <utility>;

iconer::net::IoResult
iconer::net::Startup()
noexcept
{
	::WSADATA version_data{};

	const int startup = ::WSAStartup(MAKEWORD(2, 2), std::addressof(version_data));
	if (0 != startup)
	{
		return std::unexpected{ AcquireNetworkError() };
	}
	else
	{
		return {};
	}
}

iconer::net::IoResult
iconer::net::Cleanup()
noexcept
{
	if (0 != ::WSACleanup())
	{
		return std::unexpected{ AcquireNetworkError() };
	}
	else
	{
		return {};
	}
}

iconer::net::ErrorCode
iconer::net::AcquireNetworkError()
noexcept
{
	return static_cast<ErrorCode>(::WSAGetLastError());
}

std::int32_t
iconer::net::AcquireNetworkErrorByInteger()
noexcept
{
	return ::WSAGetLastError();
}
