export module Iconer.Net.Socket:SocketOption;
import <cstdint>;

export namespace iconer::net
{
	enum class [[nodiscard]] SocketOptions : std::int32_t
	{
		Debug = 0x0001, // SO_DEBUG
		Recyclable = 0x0004, // SO_REUSEADDR
		DontRoute = 0x0010, // SO_DONTROUTE
		Broadcast = 0x0020, // SO_BROADCAST
		UseLoopback = 0x0040, // SO_USELOOPBACK
		Linger = 0x0080, // SO_LINGER
		NoDelay = 0x0001, // TCP_NODELAY
		DontLinger = (int)(~0x0080), // SO_DONTLINGER
		KeepAlive = 0x0008, // SO_KEEPALIVE
		UpdateContext = 0x700B // SO_UPDATE_ACCEPT_CONTEXT (MSWSock.h)
	};
}
