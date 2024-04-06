#pragma once
#ifndef SAGAFRAMEWORK_UTIL_BYTE_H
#define SAGAFRAMEWORK_UTIL_BYTE_H

#include <type_traits>
#include <concepts>
#include <bit>

namespace saga
{
	inline constexpr unsigned char LastByte = 0XFFU;

	[[nodiscard]]
	constexpr bool MakeSigned(bool value) noexcept
	{
		return value;
	}

	[[nodiscard]]
	constexpr bool MakeUnsigned(bool value) noexcept
	{
		return value;
	}

	template <typename T>
	[[nodiscard]]
	constexpr auto MakeSigned(T&& value) noexcept
	{
		return static_cast<std::make_signed_t<std::remove_cvref_t<T>>>(std::forward<T>(value));
	}

	template <typename T>
	[[nodiscard]]
	constexpr auto MakeUnsigned(T&& value) noexcept
	{
		return static_cast<std::make_unsigned_t<std::remove_cvref_t<T>>>(std::forward<T>(value));
	}

	template <std::integral T>
	[[nodiscard]]
	constexpr unsigned char RShift(const T& value, const size_t& times) noexcept
	{
		return static_cast<unsigned char>(value >> (times * 8ULL)) & LastByte;
	}

	using std::endian;

	[[nodiscard]]
	consteval bool IsBigEndian() noexcept
	{
		return std::endian::native == std::endian::big;
	}

	[[nodiscard]]
	consteval bool IsLittleEndian() noexcept
	{
		return std::endian::native == std::endian::little;
	}

	[[nodiscard]]
	consteval bool IsMixedEndian() noexcept
	{
		return std::endian::native != std::endian::big and std::endian::native != std::endian::little;
	}
}

#endif // !SAGAFRAMEWORK_UTIL_BYTE_H
