module;
#include <WinSock2.h>
module Iconer.Net.ErrorCode;

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
