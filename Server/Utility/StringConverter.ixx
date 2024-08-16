module;
#include <cstdint>
#include <type_traits>
#include <memory>
#include <string_view>
#include <string>

export module Iconer.Utility.StringConverter;
import Iconer.Utility.TypeTraits;

export namespace iconer::util
{
	template<size_t Len>
	[[nodiscard]]
	constexpr std::string ToString(const wchar_t(&string)[Len]) noexcept(Len < 8)
	{
		std::string result{};
		result.reserve(Len * 2 + 1);

		auto it = result.data();

		constexpr std::uint8_t byte_fix = 0xFF;

		for (const wchar_t& narrow_char : string)
		{
			const auto mid = static_cast<std::uint16_t>(narrow_char);

			constexpr std::uint16_t lhs_fix = 0xFF00;
			constexpr std::uint16_t rhs_fix = 0x00FF;

			const std::uint16_t lhs = mid & lhs_fix;
			const std::uint16_t rhs = mid & rhs_fix;
			const std::uint8_t post_lhs = static_cast<std::uint8_t>(lhs) & byte_fix;
			const std::uint8_t post_rhs = static_cast<std::uint8_t>(rhs) & byte_fix;

			result.push_back(static_cast<char>(post_lhs));
			result.push_back(static_cast<char>(post_rhs));

			(void)++it;
		}

		return result;
	}

	[[nodiscard]]
	constexpr std::string ToString(const std::wstring& string)
	{
		std::string result{};
		result.reserve(string.length() * 2 + 1);

		constexpr std::uint8_t byte_fix = 0xFF;

		for (const wchar_t& narrow_char : string)
		{
			if (0 == narrow_char)
			{
				result.push_back(0);
				break;
			}

			const auto mid = static_cast<std::uint16_t>(narrow_char);

			constexpr std::uint16_t lhs_fix = 0xFF00;
			constexpr std::uint16_t rhs_fix = 0x00FF;

			const std::uint16_t lhs = mid;
			const std::uint16_t rhs = mid >> 1;
			const std::uint8_t post_lhs = static_cast<std::uint8_t>(lhs & byte_fix);
			const std::uint8_t post_rhs = static_cast<std::uint8_t>(rhs & byte_fix);

			result.push_back(static_cast<char>(post_lhs));
			result.push_back(static_cast<char>(post_rhs));
		}

		return result;
	}

	[[nodiscard]]
	constexpr std::string ToString(std::wstring&& string)
	{
		const std::wstring origin = std::move(string);

		return ToString(origin);
	}

	[[nodiscard]]
	constexpr std::wstring ToWideString(const char* string, const size_t length)
	{
		std::wstring result{};
		result.reserve(length + 1);

		auto it = result.data();

		for (auto sit = string; sit != string + length; ++sit)
		{
			const char& narrow_char = *sit;

			(*it) = static_cast<wchar_t>(narrow_char);

			(void)++it;
		}

		return result;
	}

	template<size_t Len>
	[[nodiscard]]
	constexpr std::wstring ToWideString(const char(&string)[Len])
	{
		return ToWideString(string, Len);
	}

	[[nodiscard]]
	constexpr std::wstring ToWideString(const std::string& string)
	{
		return ToWideString(string.data(), string.length());
	}

	[[nodiscard]]
	constexpr std::wstring ToWideString(std::string&& string)
	{
		const std::string origin = std::move(string);

		return ToWideString(origin);
	}
}
