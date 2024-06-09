export module Iconer.Net.InternetProtocol;
import <cstdint>;
import <string>;
import <format>;

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

export template<>
struct std::formatter<iconer::net::InternetProtocol, char>
{
	format_parse_context::iterator parse(format_parse_context& context) const;
	format_context::iterator format(iconer::net::InternetProtocol code, format_context& context) const;
};

export template<>
struct std::formatter<iconer::net::InternetProtocol, wchar_t>
{
	wformat_parse_context::iterator parse(wformat_parse_context& context) const;
	wformat_context::iterator format(iconer::net::InternetProtocol code, wformat_context& context) const;
};

std::format_parse_context::iterator
std::formatter<iconer::net::InternetProtocol, char>::parse(std::format_parse_context& context)
const
{
	auto it = context.begin();
	const auto end = context.end();
	if (it == end or *it != '{')
	{
		throw std::format_error{ "Invalid format string." };
	}

	++it;
	if (it != end and *it != '}')
	{
		throw std::format_error{ "Missing '}' in format string." };
	}

	return it;
}

std::format_context::iterator
std::formatter<iconer::net::InternetProtocol, char>::format(iconer::net::InternetProtocol code, std::format_context& context)
const
{
	return std::format_to(context.out(), "{}", std::to_string(code));
}

std::wformat_parse_context::iterator
std::formatter<iconer::net::InternetProtocol, wchar_t>::parse(std::wformat_parse_context& context)
const
{
	auto it = context.begin();
	const auto end = context.end();
	if (it == end or *it != 'L{')
	{
		throw std::format_error{ "Invalid format string." };
	}

	++it;
	if (it != end and *it != L'}')
	{
		throw std::format_error{ "Missing '}' in format string." };
	}

	return it;
}

std::wformat_context::iterator
std::formatter<iconer::net::InternetProtocol, wchar_t>::format(iconer::net::InternetProtocol code, std::wformat_context& context)
const
{
	return std::format_to(context.out(), L"{}", std::to_wstring(code));
}
