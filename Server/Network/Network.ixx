export module Iconer.Net;
export import Iconer.Net.ErrorCode;
import <expected>;

export namespace iconer::net
{
	/// <returns>
	/// WSAEFAULT, WSASYSNOTREADY, WSAVERNOTSUPPORTED, WSAEINPROGRESS, WSAEPROCLIM
	/// </returns>
	std::expected<void, iconer::net::ErrorCode> Startup() noexcept;
	/// <returns>
	/// WSANOTINITIALISED, WSAENETDOWN, WSAEINPROGRESS
	/// </returns>
	std::expected<void, iconer::net::ErrorCode> Cleanup() noexcept;
}
