export module Iconer.Net;
export import Iconer.Net.IoResult;

export namespace iconer::net
{
	/// <returns>
	/// WSAEFAULT, WSASYSNOTREADY, WSAVERNOTSUPPORTED, WSAEINPROGRESS, WSAEPROCLIM
	/// </returns>
	IoResult Startup() noexcept;
	/// <returns>
	/// WSANOTINITIALISED, WSAENETDOWN, WSAEINPROGRESS
	/// </returns>
	IoResult Cleanup() noexcept;
}
