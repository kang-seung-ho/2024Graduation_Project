export module Iconer.Net.IoCategory;
import <cstdint>;
import <string>;
import <format>;

export namespace iconer::net
{
	enum class [[nodiscard]] IoCategory : std::uint32_t
	{
		Synchronous = 0b000000000U,
		Asynchronous = 0b000000001U,
		Registered = 0b100000000U,
	};
}

export namespace std
{
	[[nodiscard]]
	constexpr string to_string(const iconer::net::IoCategory& category)
	{
		switch (category)
		{
			case iconer::net::IoCategory::Synchronous: return "Synchronous";
			case iconer::net::IoCategory::Asynchronous: return "Asynchronous";
			case iconer::net::IoCategory::Registered: return "Registered";
			default: return "Unknown";
		}
	}

	[[nodiscard]]
	constexpr string to_string(iconer::net::IoCategory&& category)
	{
		switch (std::move(category))
		{
			case iconer::net::IoCategory::Synchronous: return "Synchronous";
			case iconer::net::IoCategory::Asynchronous: return "Asynchronous";
			case iconer::net::IoCategory::Registered: return "Registered";
			default: return "Unknown";
		}
	}
	[[nodiscard]]
	constexpr wstring to_wstring(const iconer::net::IoCategory& category)
	{
		switch (category)
		{
			case iconer::net::IoCategory::Synchronous: return L"Synchronous";
			case iconer::net::IoCategory::Asynchronous: return L"Asynchronous";
			case iconer::net::IoCategory::Registered: return L"Registered";
			default: return L"Unknown";
		}
	}

	[[nodiscard]]
	constexpr wstring to_wstring(iconer::net::IoCategory&& category)
	{
		switch (std::move(category))
		{
			case iconer::net::IoCategory::Synchronous: return L"Synchronous";
			case iconer::net::IoCategory::Asynchronous: return L"Asynchronous";
			case iconer::net::IoCategory::Registered: return L"Registered";
			default: return L"Unknown";
		}
	}
}

/// <summary>
/// {i} : format to int32_t
/// </summary>
export template<>
struct std::formatter<iconer::net::IoCategory, char>
{
	static constexpr format_parse_context::iterator
		parse(format_parse_context& context)
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

	static format_context::iterator
		format(const iconer::net::IoCategory& category, format_context& context)
	{
		return std::format_to(context.out(), "Io Category<{}>", to_string(category));
	}

	static format_context::iterator
		format(iconer::net::IoCategory&& category, format_context& context)
	{
		return std::format_to(context.out(), "Io Category<{}>", to_string(std::move(category)));
	}
};

/// <summary>
/// {i} : format to int32_t
/// </summary>
export template<>
struct std::formatter<iconer::net::IoCategory, wchar_t>
{
	static constexpr wformat_parse_context::iterator
		parse(wformat_parse_context& context)
	{
		auto it = context.begin();
		const auto end = context.end();
		if (it == end or *it != L'{')
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

	static wformat_context::iterator
		format(const iconer::net::IoCategory& category, wformat_context& context)
	{
		return std::format_to(context.out(), L"Io Category<{}>", to_wstring(category));
	}

	static wformat_context::iterator
		format(iconer::net::IoCategory&& category, wformat_context& context)
	{
		return std::format_to(context.out(), L"Io Category<{}>", to_wstring(std::move(category)));
	}
};
