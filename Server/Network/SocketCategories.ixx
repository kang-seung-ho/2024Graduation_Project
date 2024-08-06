export module Iconer.Net.Socket:SocketCategory;
import <cstdint>;
import <string>;

export namespace iconer::net
{
	enum class [[nodiscard]] SocketCategory : std::uint32_t
	{
		Synchronous = 0b000000000U,
		Asynchronous = 0b000000001U,
		Registered = 0b100000000U,
	};
}

export namespace std
{
	[[nodiscard]]
	constexpr string to_string(iconer::net::SocketCategory category)
	{
		switch (category)
		{
		case iconer::net::SocketCategory::Synchronous: return "Synchronous";
		case iconer::net::SocketCategory::Asynchronous: return "Asynchronous";
		case iconer::net::SocketCategory::Registered: return "Registered";
		default: return "Unknown";
		}
	}

	[[nodiscard]]
	constexpr wstring to_wstring(iconer::net::SocketCategory category)
	{
		switch (category)
		{
		case iconer::net::SocketCategory::Synchronous: return L"Synchronous";
		case iconer::net::SocketCategory::Asynchronous: return L"Asynchronous";
		case iconer::net::SocketCategory::Registered: return L"Registered";
		default: return L"Unknown";
		}
	}
}
