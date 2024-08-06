export module Iconer.Utility.StringConverter;
export import :CharacterReader;
import Iconer.Utility.Byte;
import <utility>;
import <bit>;
import <string>;
import <string_view>;
import <iterator>;
import <algorithm>;

export namespace iconer::util
{
	[[nodiscard]]
	constexpr std::string ConvertToString(std::u8string_view string)
	{
		std::string result{};
		result.reserve(string.length());

		auto it = std::back_inserter(result);

		for (const char8_t& uch : string)
		{
			*(it++) = static_cast<std::uint32_t>(uch);
		}

		return result;
	}

	[[nodiscard]]
	constexpr std::string ConvertToString(std::wstring_view string)
	{
		std::string result{};
		result.reserve(string.length() * 2);

		auto it = std::back_inserter(result);

		for (const wchar_t& wch : string)
		{
			const auto longer = static_cast<std::uint32_t>(wch);
			*(it++) = static_cast<char>(RShift(longer, 0));
			*(it++) = static_cast<char>(RShift(longer, 1));
		}

		return result;
	}

	[[nodiscard]]
	constexpr std::string ConvertToString(std::u16string_view string)
	{
		std::string result{};
		result.reserve(string.length());

		auto it = std::back_inserter(result);

		for (const char16_t& uch : string)
		{
			const auto longer = static_cast<std::uint32_t>(uch);
			*(it++) = static_cast<char>(RShift(longer, 0));
			*(it++) = static_cast<char>(RShift(longer, 1));
		}

		return result;
	}

	[[nodiscard]]
	constexpr std::string ConvertToString(std::u32string_view string)
	{
		std::string result{};
		result.reserve(string.length());

		auto it = std::back_inserter(result);

		for (const char32_t& uch : string)
		{
			const auto longer = static_cast<std::uint64_t>(uch);
			*(it++) = static_cast<char>(RShift(longer, 0));
			*(it++) = static_cast<char>(RShift(longer, 1));
			*(it++) = static_cast<char>(RShift(longer, 2));
			*(it++) = static_cast<char>(RShift(longer, 3));
		}

		return result;
	}

	[[nodiscard]]
	constexpr std::wstring ConvertToWideString(std::string_view string)
	{
		std::wstring result{};
		result.reserve(string.length() / 3 * 2); // factor is 0.75

		auto it = std::back_inserter(result);

		auto begin = string.data();
		auto end = begin + string.length();

		wchar_t wch{};
		while (begin < end)
		{
			begin = ReadChar(begin, wch);
			*(it++) = wch;
		}

		return result;
	}
}
