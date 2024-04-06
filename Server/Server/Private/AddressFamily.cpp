module Iconer.Net.IpAddressFamily;
import <string_view>;
import <utility>;
import <algorithm>;
import <ranges>;

std::format_context::iterator
std::formatter<iconer::net::IpAddressFamily, char>::format(const iconer::net::IpAddressFamily& fm, std::format_context& context)
const noexcept
{
	static auto myprint = [&](std::string_view caption) noexcept {
		if (isUpperCase)
		{
			static constexpr auto myupper = [](const char& c) noexcept -> char {
				return c + 32;
			};

			const auto view = caption | std::ranges::views::transform(myupper);
			auto it = std::ranges::copy(view, context.out());

			return it.out;
		}
		else
		{
			return std::format_to(context.out(), "{}", caption);
		}
	};

	switch (fm)
	{
		case iconer::net::IpAddressFamily::IPv4:
		{
			if (isStrengthened)
			{
				return myprint("Internet Protocol v4 Familiy");
			}
			else if (isSimplified)
			{
				return myprint("IPv4");
			}
			else
			{
				return myprint("IPv4 Familiy");
			}
		}

		case iconer::net::IpAddressFamily::IPv6:
		{
			if (isStrengthened)
			{
				return myprint("Internet Protocol v6 Familiy");
			}
			else if (isSimplified)
			{
				return myprint("IPv6");
			}
			else
			{
				return myprint("IPv6 Familiy");
			}
		}

		default:
		{
			if (isStrengthened)
			{
				return myprint("Unknown Internet Protocol Address Family");
			}
			else if (isSimplified)
			{
				return myprint("Unknown");
			}
			else
			{
				return myprint("Unknown Address Familiy");
			}
		}
	}
}

std::wformat_context::iterator
std::formatter<iconer::net::IpAddressFamily, wchar_t>::format(const iconer::net::IpAddressFamily& fm, std::wformat_context& context)
const noexcept
{
	static auto myprint = [&](std::wstring_view caption) noexcept {
		if (isUpperCase)
		{
			static constexpr auto myupper = [](const wchar_t& c) noexcept -> wchar_t {
				return c + 32;
			};

			const auto view = caption | std::ranges::views::transform(myupper);
			auto it = std::ranges::copy(view, context.out());

			return it.out;
		}
		else
		{
			return std::format_to(context.out(), L"{}", caption);
		}
	};

	switch (fm)
	{
		case iconer::net::IpAddressFamily::IPv4:
		{
			if (isStrengthened)
			{
				return myprint(L"Internet Protocol v4 Familiy");
			}
			else if (isSimplified)
			{
				return myprint(L"IPv4");
			}
			else
			{
				return myprint(L"IPv4 Familiy");
			}
		}

		case iconer::net::IpAddressFamily::IPv6:
		{
			if (isStrengthened)
			{
				return myprint(L"Internet Protocol v6 Familiy");
			}
			else if (isSimplified)
			{
				return myprint(L"IPv6");
			}
			else
			{
				return myprint(L"IPv6 Familiy");
			}
		}

		default:
		{
			if (isStrengthened)
			{
				return myprint(L"Unknown Internet Protocol Address Family");
			}
			else if (isSimplified)
			{
				return myprint(L"Unknown");
			}
			else
			{
				return myprint(L"Unknown Address Familiy");
			}
		}
	}
}
