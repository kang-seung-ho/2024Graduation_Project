module;
#pragma warning(push)
#pragma warning(disable : 4006)
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#include <WinSock2.h>
#pragma warning(pop)

module Iconer.Net;
import Iconer.Net.ErrorCode;
import <utility>;

std::optional<iconer::net::ErrorCode>
iconer::net::Startup()
noexcept
{
	::WSADATA version_data{};

	const int startup = ::WSAStartup(MAKEWORD(2, 2), std::addressof(version_data));
	if (0 != startup)
	{
		return AcquireNetworkError();
		//return false;
	}
	else
	{
		return std::nullopt;
		//return true;
	}
}

std::optional<iconer::net::ErrorCode>
iconer::net::Cleanup()
noexcept
{
	if (0 != ::WSACleanup())
	{
		return AcquireNetworkError();
		//return false;
	}
	else
	{
		return std::nullopt;
		//return true;
	}
}
