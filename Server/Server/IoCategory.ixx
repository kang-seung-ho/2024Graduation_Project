export module Iconer.Net.IoCategory;
import <cstdint>;
import <string>;

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
