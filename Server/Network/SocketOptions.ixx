export module Iconer.Net.Socket:SocketOption;
import <cstdint>;

export namespace iconer::net
{
	enum class [[nodiscard]] SocketOption : std::int32_t
	{
		None = 0,

		/// <summary>
		/// TCP_NODELAY
		/// </summary>
		NoDelay = 0x0001,

		/// <summary>
		/// SO_DEBUG
		/// </summary>
		Debug = 0x0001,
		/// <summary>
		/// SO_REUSEADDR
		/// </summary>
		Recyclable = 0x0004,
		/// <summary>
		/// SO_DONTROUTE
		/// </summary>
		DontRoute = 0x0010,
		/// <summary>
		/// SO_BROADCAST
		/// </summary>
		Broadcast = 0x0020,
		/// <summary>
		/// SO_USELOOPBACK
		/// </summary>
		UseLoopback = 0x0040,
		/// <summary>
		/// SO_LINGER
		/// </summary>
		Linger = 0x0080,
		/// <summary>
		/// SO_DONTLINGER
		/// </summary>
		DontLinger = (std::int32_t)(~0x0080),
		/// <summary>
		/// SO_KEEPALIVE
		/// </summary>
		KeepAlive = 0x0008,
		/// <summary>
		/// SO_UPDATE_ACCEPT_CONTEXT (MSWSock.h)
		/// </summary>
		UpdateContext = 0x700B
	};
}
