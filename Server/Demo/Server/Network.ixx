export module Iconer.Net;
export import Iconer.Net.ErrorCode;
import <optional>;

export namespace iconer::net
{
	/// <returns>
	/// WSAEFAULT, WSASYSNOTREADY, WSAVERNOTSUPPORTED, WSAEINPROGRESS, WSAEPROCLIM
	/// </returns>
	std::optional<ErrorCode> Startup() noexcept;
	/// <returns>
	/// WSANOTINITIALISED, WSAENETDOWN, WSAEINPROGRESS
	/// </returns>
	std::optional<ErrorCode> Cleanup() noexcept;
}
