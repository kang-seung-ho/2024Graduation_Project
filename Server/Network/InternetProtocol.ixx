export module Iconer.Net.InternetProtocol;
import <cstdint>;
import <string>;

export namespace iconer::net
{
	enum class [[nodiscard]] InternetProtocol : std::int32_t
	{
		Unknown, TCP, UDP
	};
}

export namespace std
{
	[[nodiscard]]
	constexpr string to_string(iconer::net::InternetProtocol protocol)
	{
		switch (protocol)
		{
		case iconer::net::InternetProtocol::TCP: return "TCP";
		case iconer::net::InternetProtocol::UDP: return "UDP";
		default: return "Unknown";
		}
	}

	[[nodiscard]]
	constexpr wstring to_wstring(iconer::net::InternetProtocol protocol)
	{
		switch (protocol)
		{
		case iconer::net::InternetProtocol::TCP: return L"TCP";
		case iconer::net::InternetProtocol::UDP: return L"UDP";
		default: return L"Unknown";
		}
	}
}
