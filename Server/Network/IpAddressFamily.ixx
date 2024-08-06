export module Iconer.Net.IpAddressFamily;
import <cstdint>;
import <string>;

export namespace iconer::net
{
	enum class [[nodiscard]] IpAddressFamily : std::uint16_t
	{
		Unknown, IPv4 = 2, IPv6 = 23
	};
}

export namespace std
{
	[[nodiscard]]
	constexpr std::string to_string(const iconer::net::IpAddressFamily& fm)
	{
		switch (fm)
		{
		case iconer::net::IpAddressFamily::IPv4:
		{
			return "IPv4";
		}

		case iconer::net::IpAddressFamily::IPv6:
		{
			return "IPv6";
		}

		default:
		{
			return "Unknown";
		}
		}
	}

	[[nodiscard]]
	constexpr std::wstring to_wstring(const iconer::net::IpAddressFamily& fm)
	{
		switch (fm)
		{
		case iconer::net::IpAddressFamily::IPv4:
		{
			return L"IPv4";
		}

		case iconer::net::IpAddressFamily::IPv6:
		{
			return L"IPv6";
		}

		default:
		{
			return L"Unknown";
		}
		}
	}
}
