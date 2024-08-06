module;
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ws2ipdef.h>
#include <MSWSock.h>

#define LIKELY [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Net.Socket;
import Iconer.Net.IpAddress;
import Iconer.Net.EndPoint;
import Iconer.Net.IoContext;
