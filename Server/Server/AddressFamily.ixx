export module Iconer.Net.IpAddressFamily;
import <string>;
import <format>;

export namespace iconer::net
{
	enum class [[nodiscard]] IpAddressFamily
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

export template<>
struct std::formatter<iconer::net::IpAddressFamily, char>
{
	constexpr std::format_parse_context::iterator
		parse(std::format_parse_context& context)
	{
		auto it = context.begin();
		const auto end = context.end();
		if (it == end or *it != '{')
		{
			throw std::format_error{ "Invalid format string." };
		}

		bool escaped = false;
		do
		{
			if (*it == '}')
			{
				escaped = true;
			}

			if (*it == 'U' or *it == 'u')
			{
				isUpperCase = true;
			}
			else if (*it == 'e')
			{
				isSimplified = true;
			}
			else if (*it == 'E')
			{
				isUpperCase = true;
				isSimplified = true;
			}
			else if (*it == 'g')
			{
				isStrengthened = true;
			}
			else if (*it == 'G')
			{
				isUpperCase = true;
				isStrengthened = true;
			}

			++it;
		}
		while (it + 1 != context.end());

		if (not escaped and *it != '}')
		{
			throw std::format_error{ "Missing '}' in format string." };
		}

		return it;
	}

	std::format_context::iterator
		format(const iconer::net::IpAddressFamily& fm, std::format_context& context)
		const noexcept;

private:
	bool isSimplified = false;   // e, E
	bool isStrengthened = false; // g, G
	bool isUpperCase = false;    // U
};

export template<>
struct std::formatter<iconer::net::IpAddressFamily, wchar_t>
{
	constexpr std::wformat_parse_context::iterator
		parse(std::wformat_parse_context& context)
	{
		auto it = context.begin();
		const auto end = context.end();
		if (it == end or *it != L'{')
		{
			throw std::format_error{ "Invalid format string." };
		}

		bool escaped = false;
		do
		{
			if (*it == L'}')
			{
				escaped = true;
			}

			if (*it == L'U' or *it == L'u')
			{
				isUpperCase = true;
			}
			else if (*it == L'e')
			{
				isSimplified = true;
			}
			else if (*it == L'E')
			{
				isUpperCase = true;
				isSimplified = true;
			}
			else if (*it == L'g')
			{
				isStrengthened = true;
			}
			else if (*it == L'G')
			{
				isUpperCase = true;
				isStrengthened = true;
			}

			++it;
		}
		while (it + 1 != context.end());

		if (not escaped and *it != L'}')
		{
			throw std::format_error{ "Missing '}' in format string." };
		}

		return it;
	}

	std::wformat_context::iterator
		format(const iconer::net::IpAddressFamily& fm, std::wformat_context& context)
		const noexcept;

private:
	bool isSimplified = false;   // e, E
	bool isStrengthened = false; // g, G
	bool isUpperCase = false;    // U
};
