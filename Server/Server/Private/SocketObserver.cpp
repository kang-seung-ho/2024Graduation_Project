module;
#include <WinSock2.h>

module Iconer.Net.Socket;

using namespace iconer;

bool
net::Socket::IsAvailable()
const noexcept
{
	return myHandle != INVALID_SOCKET;
}
