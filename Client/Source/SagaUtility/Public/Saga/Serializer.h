#pragma once
#ifndef SAGAFRAMEWORK_UTIL_SERIALIZER_H
#define SAGAFRAMEWORK_UTIL_SERIALIZER_H

#include <CoreMinimal.h>
#include "Saga/Constraints.h"
#include "Saga/Byte.h"
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <algorithm>
#include <bit>
#include <memory>
#include <tuple>
#include <string>
#include <string_view>
#include <utility>

namespace saga::inline serializers
{
	/// <summary>Transfer a boolean value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const bool& value);

	/// <summary>Allocate a byte buffer for a boolean value</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const bool& value);

	/// <summary>Transfer a character to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const char& value);

	/// <summary>Allocate a byte buffer for a character</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const char& value);

	/// <summary>Transfer an unsigned character to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const unsigned char& value);

	/// <summary>Allocate a byte buffer for an unsigned character</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const unsigned char& value);

	/// <summary>Transfer an utf-8 character to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const char8_t& value);

	/// <summary>Allocate a byte buffer for an utf-8 character</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const char8_t& value);

	/// <summary>Transfer a 16-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const int16& value);

	/// <summary>Allocate a byte buffer for a 16-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const int16& value);

	/// <summary>Transfer an unsigned 16-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const uint16& value);

	/// <summary>Allocate a byte buffer for an unsigned 16-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const uint16& value);

	/// <summary>Transfer an utf-16 character to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const char16_t& value);

	/// <summary>Allocate a byte buffer for an utf-16 character</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const char16_t& value);

	/// <summary>Transfer a wide character to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const wchar_t& value);

	/// <summary>Allocate a byte buffer for a wide character</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const wchar_t& value);

	/// <summary>Transfer a 32-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const int32& value);

	/// <summary>Allocate a byte buffer for a 32-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const int32& value);

	/// <summary>Transfer an unsigned 32-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const uint32& value);

	/// <summary>Allocate a byte buffer for  an unsigned 32-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const uint32& value);

	/// <summary>Transfer a 32-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const long& value);

	/// <summary>Allocate a byte buffer for a 32-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const long& value);

	/// <summary>Transfer an unsigned 32-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const unsigned long& value);

	/// <summary>Allocate a byte buffer for an unsigned 32-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const unsigned long& value);

	/// <summary>Transfer an utf-32 character to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const char32_t& value);

	/// <summary>Allocate a byte buffer for an utf-32 character</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const char32_t& value);

	/// <summary>Transfer a 64-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const int64& value);

	/// <summary>Allocate a byte buffer for a 64-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const int64& value);

	/// <summary>Transfer an unsigned 64-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const uint64& value);

	/// <summary>Allocate a byte buffer for an unsigned 64-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const uint64& value);

	/// <summary>Transfer a 32-bit floating point value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const float_t& value);

	/// <summary>Allocate a byte buffer for a 32-bit floating point value</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const float_t& value);

	/// <summary>Transfer a 32-bit floating point value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const double_t& value);

	/// <summary>Allocate a byte buffer for a 64-bit floating point value</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const double_t& value);

	/// <summary>Transfer a string to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const std::string_view str);

	/// <summary>Allocate a byte buffer for a string</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const std::string_view str);

	/// <summary>Transfer a wide string to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const std::wstring_view str);

	/// <summary>Allocate a byte buffer for a wide string</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const std::wstring_view str);

	/// <summary>Transfer an utf-8 string to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const std::u8string_view str);

	/// <summary>Allocate a byte buffer for an utf-8 string</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const std::u8string_view str);

	/// <summary>Transfer a wide string to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const std::u16string_view str);

	/// <summary>Allocate a byte buffer for a string</summary>
	/// <exception cref="std::bad_alloc"/>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const std::u16string_view str);

	/// <summary>Transfer a wide string to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	static std::byte* Serialize(std::byte* dest, const std::u32string_view str);

	/// <summary>Allocate a byte buffer for a enumeration</summary>
	/// <exception cref="std::bad_alloc"/>
	template<enumerations T>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const T& value)
	{
		return Serialize(static_cast<std::underlying_type_t<T>>(value));
	}

	/// <summary>Transfer a enumeration to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	template<enumerations T>
	static std::byte* Serialize(std::byte* dest, const T& value)
	{
		return Serialize(dest, static_cast<std::underlying_type_t<T>>(value));
	}
}

namespace saga::detail
{
	template<typename Tuple, size_t... Indices>
	std::byte* Serialize(std::byte* dest, Tuple&& tuple, std::index_sequence<Indices...>)
	{
		std::byte* it = dest;

		((it = saga::Serialize(it, std::get<Indices>(std::forward<Tuple>(tuple)))), ...);

		return it;
	}

	template<typename Arg>
	constexpr size_t GetItemByteSize(Arg&& arg) noexcept
	{
		if constexpr (std::is_same_v<clean_t<Arg>, FString>)
		{
			return arg.Len() * sizeof(TCHAR);
		}
		else if constexpr (std::is_same_v<clean_t<Arg>, FName>)
		{
			return arg.GetStringLength() * sizeof(TCHAR);
		}
		else if constexpr (std::is_same_v<clean_t<Arg>, std::string>)
		{
			return arg.size();
		}
		else if constexpr (std::is_same_v<clean_t<Arg>, std::wstring>)
		{
			return arg.size() * sizeof(wchar_t);
		}
		if constexpr (std::is_same_v<clean_t<Arg>, std::string_view>)
		{
			return arg.size();
		}
		else if constexpr (std::is_same_v<clean_t<Arg>, std::wstring_view>)
		{
			return arg.size() * sizeof(wchar_t);
		}
		else
		{
			return sizeof(Arg);
		}
	}

	template<typename Tuple, size_t... Indices>
	constexpr size_t GetByteSize(const Tuple& tuple, std::index_sequence<Indices...>) noexcept
	{
		size_t result{};

		((result += GetItemByteSize(std::get<Indices>(tuple))), ...);

		return result;
	}
}

namespace saga
{
	/// <summary>Allocate a byte buffer for a copied tuple</summary>
	/// <exception cref="std::bad_alloc"/>
	template<typename... Args>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(const std::tuple<Args...>& tuple);

	/// <summary>Transfer a copied tuple to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	template<typename... Args>
	static std::byte* Serialize(std::byte* dest, const std::tuple<Args...>& tuple);

	/// <summary>Allocate a byte buffer for a moved tuple</summary>
	/// <exception cref="std::bad_alloc"/>
	template<typename... Args>
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serialize(std::tuple<Args...>&& tuple);

	/// <summary>Transfer a moved tuple to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	template<typename... Args>
	static std::byte* Serialize(std::byte* dest, std::tuple<Args...>&& tuple);

	/// <summary>Allocate a byte buffer of multiple values</summary>
	/// <exception cref="std::bad_alloc"/>
	template<typename... Args>
		requires (1 < sizeof...(Args))
	[[nodiscard]]
	static std::unique_ptr<std::byte[]> Serializes(Args&&... args);

	/// <summary>Transfer arguments to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	template<typename... Args>
		requires (0 < sizeof...(Args))
	static std::byte* Serializes(std::byte* dest, Args&&... args);
}

namespace saga::inline deserializers
{
	/// <summary>Read a boolean value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, bool& output)
	{
		output = std::bit_cast<bool>(buffer[0]);

		return buffer + sizeof(bool);
	}

	/// <summary>Read a character from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, char& output)
	{
		output = std::bit_cast<char>(buffer[0]);

		return buffer + sizeof(char);
	}
	
	/// <summary>Read a character from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, signed char& output)
	{
		output = std::bit_cast<signed char>(buffer[0]);

		return buffer + sizeof(signed char);
	}

	/// <summary>Read an unsigned character from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, unsigned char& output)
	{
		output = std::bit_cast<unsigned char>(buffer[0]);

		return buffer + sizeof(unsigned char);
	}

	/// <summary>Read an utf-8 character from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, char8_t& output)
	{
		output = std::bit_cast<char8_t>(buffer[0]);

		return buffer + sizeof(char8_t);
	}

	/// <summary>Read a 16-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, int16& output)
	{
		const uint32 mid = (static_cast<uint32>(static_cast<uint8>(buffer[1])) << 8U)
			| static_cast<uint32>(static_cast<uint8>(buffer[0]));

		output = static_cast<int16>(mid);

		return buffer + sizeof(int16);
	}

	/// <summary>Read an unsigned 16-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, uint16& output)
	{
		const uint32 mid = (static_cast<uint32>(static_cast<uint8>(buffer[1])) << 8U)
			| static_cast<uint32>(static_cast<uint8>(buffer[0]));

		output = static_cast<uint16>(mid);

		return buffer + sizeof(uint16);
	}

	/// <summary>Read a wide character from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, wchar_t& output)
	{
		const uint32 mid = (static_cast<uint32>(static_cast<uint8>(buffer[1])) << 8U)
			| static_cast<uint32>(static_cast<uint8>(buffer[0]));

		output = static_cast<wchar_t>(mid);

		return buffer + sizeof(wchar_t);
	}

	/// <summary>Read an utf-16 character from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, char16_t& output)
	{
		const uint32 mid = (static_cast<uint32>(static_cast<uint8>(buffer[1])) << 8U)
			| static_cast<uint32>(static_cast<uint8>(buffer[0]));

		output = static_cast<char16_t>(mid);

		return buffer + sizeof(char16_t);
	}

	/// <summary>Read a 32-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, int32& output)
	{
		const uint64 mid = (static_cast<uint64>(static_cast<uint8>(buffer[3])) << 24U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[2])) << 16U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[1])) << 8U)
			| static_cast<uint64>(static_cast<uint8>(buffer[0]));

		output = static_cast<int32>(mid);

		return buffer + sizeof(int32);
	}

	/// <summary>Read an unsigned 32-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, uint32& output)
	{
		const uint64 mid = (static_cast<uint64>(static_cast<uint8>(buffer[3])) << 24U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[2])) << 16U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[1])) << 8U)
			| static_cast<uint64>(static_cast<uint8>(buffer[0]));

		output = static_cast<uint32>(mid);

		return buffer + sizeof(uint32);
	}

	/// <summary>Read a 32-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, long& output)
	{
		const uint64 mid = (static_cast<uint64>(static_cast<uint8>(buffer[3])) << 24U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[2])) << 16U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[1])) << 8U)
			| static_cast<uint64>(static_cast<uint8>(buffer[0]));

		output = static_cast<long>(mid);

		return buffer + sizeof(long);
	}

	/// <summary>Read an unsigned 32-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, unsigned long& output)
	{
		const uint64 mid = (static_cast<uint64>(static_cast<uint8>(buffer[3])) << 24U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[2])) << 16U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[1])) << 8U)
			| static_cast<uint64>(static_cast<uint8>(buffer[0]));

		output = static_cast<unsigned long>(mid);

		return buffer + sizeof(unsigned long);
	}

	/// <summary>Read an utf-32 character from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, char32_t& output)
	{
		const uint64 mid = (static_cast<uint64>(static_cast<uint8>(buffer[3])) << 24U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[2])) << 16U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[1])) << 8U)
			| static_cast<uint64>(static_cast<uint8>(buffer[0]));

		output = static_cast<char32_t>(mid);

		return buffer + sizeof(char32_t);
	}

	/// <summary>Read a 64-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, int64& output)
	{
		output = static_cast<int64>((static_cast<uint64>(static_cast<uint8>(buffer[7])) << 56U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[6])) << 48U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[5])) << 40U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[4])) << 32U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[3])) << 24U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[2])) << 16U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[1])) << 8U)
			| static_cast<uint64>(static_cast<uint8>(buffer[0])));

		return buffer + sizeof(int64);
	}

	/// <summary>Read an unsigned 64-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, uint64& output)
	{
		output = (static_cast<uint64>(static_cast<uint8>(buffer[7])) << 56U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[6])) << 48U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[5])) << 40U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[4])) << 32U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[3])) << 24U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[2])) << 16U)
			| (static_cast<uint64>(static_cast<uint8>(buffer[1])) << 8U)
			| static_cast<uint64>(static_cast<uint8>(buffer[0]));

		return buffer + sizeof(uint64);
	}

	/// <summary>Read a 32-bit floating point value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, float_t& output)
	{
		int32 mid{};
		auto it = Deserialize(buffer, mid);

		output = std::bit_cast<float_t>(mid);

		return it;
	}

	/// <summary>Read a 64-bit floating point value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, double_t& output)
	{
		int64 mid{};
		auto it = Deserialize(buffer, mid);

		output = std::bit_cast<double_t>(mid);

		return it;
	}

	/// <summary>Read a string from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static const std::byte* Deserialize(const std::byte* buffer, size_t length, std::string& output);

#ifdef __cpp_lib_char8_t
	/// <summary>Read a utf-8 string from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static const std::byte* Deserialize(const std::byte* buffer, size_t length, std::u8string& output);
#endif // __cpp_lib_char8_t

	/// <summary>Read a wide string from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static const std::byte* Deserialize(const std::byte* buffer, size_t length, std::wstring& output);

	/// <summary>Read a utf-16 string from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static const std::byte* Deserialize(const std::byte* buffer, size_t length, std::u16string& output);

	/// <summary>Read a utf-32 string from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	static const std::byte* Deserialize(const std::byte* buffer, size_t length, std::u32string& output);

	/// <summary>Read a array from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	template<size_t Length>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, size_t length, char(&output)[Length])
	{
		const std::byte* it = buffer;
		char* out = output;

		while (it < buffer + length * sizeof(char))
		{
			*out = std::bit_cast<char>(*it);

			++it; ++out;
		}

		return it;
	}

	/// <summary>Read a array from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	template<size_t Length>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, size_t length, char8_t(&output)[Length])
	{
		const std::byte* it = buffer;
		char8_t* out = output;

		while (it < buffer + length * sizeof(char8_t))
		{
			*out = std::bit_cast<char8_t>(*it);

			++it; ++out;
		}

		return it;
	}

	/// <summary>Read a array from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	template<size_t Length>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, size_t length, wchar_t(&output)[Length])
	{
		const std::byte* it = buffer;
		wchar_t* out = output;

		while (it < buffer + length * sizeof(wchar_t))
		{
			it = Deserialize(it, *out);

			++out;
		}

		return it;
	}

	/// <summary>Read a array from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	template<size_t Length>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, size_t length, char16_t(&output)[Length])
	{
		const std::byte* it = buffer;
		char16_t* out = output;

		while (it < buffer + length * sizeof(char16_t))
		{
			it = Deserialize(it, *out);

			++out;
		}

		return it;
	}

	/// <summary>Read a array from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	template<size_t Length>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, size_t length, char32_t(&output)[Length])
	{
		const std::byte* it = buffer;
		char32_t* out = output;

		while (it < buffer + length * sizeof(char32_t))
		{
			it = Deserialize(it, *out);

			++out;
		}

		return it;
	}

	/// <summary>Read a enumeration from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	template<enumerations T>
	static constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, T& output)
	{
		std::underlying_type_t<T> result{};
		auto it = Deserialize(buffer, result);
		output = static_cast<T>(result);

		return it;
	}
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const bool& value)
{
	dest[0] = std::bit_cast<std::byte>(value);
	return dest + 1;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const bool& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(bool));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const char& value)
{
	dest[0] = std::bit_cast<std::byte>(value);
	return dest + 1;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const char& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(char));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const unsigned char& value)
{
	dest[0] = std::bit_cast<std::byte>(value);
	return dest + 1;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const unsigned char& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(unsigned char));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const char8_t& value)
{
	dest[0] = std::bit_cast<std::byte>(value);
	return dest + 1;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const char8_t& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(char8_t));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const int16& value)
{
	const auto longer = static_cast<int32>(value);
	dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
	dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));

	return dest + 2;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const int16& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(int16));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const uint16& value)
{
	const auto longer = static_cast<uint32>(value);
	dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
	dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));

	return dest + 2;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const uint16& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(uint16));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const char16_t& value)
{
	const auto longer = static_cast<uint32>(value);
	dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
	dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));

	return dest + 2;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const char16_t& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(char16_t));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const wchar_t& value)
{
	const auto longer = static_cast<uint32>(value);
	dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
	dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));

	return dest + 2;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const wchar_t& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(wchar_t));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const int32& value)
{
	const auto longer = static_cast<int64>(value);
	dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
	dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));
	dest[2] = std::bit_cast<std::byte>(RShift(longer, 2));
	dest[3] = std::bit_cast<std::byte>(RShift(longer, 3));

	return dest + 4;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const int32& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(int32));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const uint32& value)
{
	const auto longer = static_cast<uint64>(value);
	dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
	dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));
	dest[2] = std::bit_cast<std::byte>(RShift(longer, 2));
	dest[3] = std::bit_cast<std::byte>(RShift(longer, 3));

	return dest + 4;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const uint32& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(uint32));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const long& value)
{
	const auto longer = static_cast<int64>(value);
	dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
	dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));
	dest[2] = std::bit_cast<std::byte>(RShift(longer, 2));
	dest[3] = std::bit_cast<std::byte>(RShift(longer, 3));

	return dest + 4;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const long& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(long));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const unsigned long& value)
{
	const auto longer = static_cast<uint64>(value);
	dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
	dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));
	dest[2] = std::bit_cast<std::byte>(RShift(longer, 2));
	dest[3] = std::bit_cast<std::byte>(RShift(longer, 3));

	return dest + 4;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const unsigned long& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(unsigned long));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const char32_t& value)
{
	const auto longer = static_cast<uint64>(value);
	dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
	dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));
	dest[2] = std::bit_cast<std::byte>(RShift(longer, 2));
	dest[3] = std::bit_cast<std::byte>(RShift(longer, 3));

	return dest + 4;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const char32_t& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(char32_t));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const int64& value)
{
	std::byte* it = dest;
	for (size_t i = 0; i < 8; ++i)
	{
		*(it++) = std::bit_cast<std::byte>(RShift(value, i));
	}

	return it;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const int64& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(int64));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const uint64& value)
{
	std::byte* it = dest;
	for (size_t i = 0; i < 8; ++i)
	{
		*(it++) = std::bit_cast<std::byte>(RShift(value, i));
	}

	return it;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const uint64& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(uint64));
	Serialize(buffer.get(), value);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const float_t& value)
{
	return Serialize(dest, std::bit_cast<int32>(value));
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const float_t& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(int32));
	Serialize(buffer.get(), std::bit_cast<int32>(value));
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const double_t& value)
{
	return Serialize(dest, std::bit_cast<int64>(value));
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const double_t& value)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(int64));
	Serialize(buffer.get(), std::bit_cast<int64>(value));
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const std::string_view str)
{
	return std::ranges::transform(str.cbegin(), str.cend(), dest
		, [](const char& ch) noexcept {
		return std::bit_cast<std::byte>(ch);
	}).out;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const std::string_view str)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(char) * str.length());
	Serialize(buffer.get(), str);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const std::wstring_view str)
{
	std::ranges::for_each(str, [&dest](const wchar_t& ch) {
		dest = Serialize(dest, ch);
	});

	return dest;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const std::wstring_view str)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(wchar_t) * str.length());
	Serialize(buffer.get(), str);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const std::u8string_view str)
{
	return std::ranges::transform(str.cbegin(), str.cend(), dest
		, [](const char8_t& ch) {
		return std::bit_cast<std::byte>(ch);
	}).out;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const std::u8string_view str)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(char8_t) * str.length());
	Serialize(buffer.get(), str);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const std::u16string_view str)
{
	std::ranges::for_each(str, [&dest](const char16_t& ch) {
		dest = Serialize(dest, ch);
	});

	return dest;
}

std::unique_ptr<std::byte[]>
saga::serializers::Serialize(const std::u16string_view str)
{
	auto buffer = std::make_unique<std::byte[]>(sizeof(char16_t) * str.length());
	Serialize(buffer.get(), str);
	return buffer;
}

std::byte*
saga::serializers::Serialize(std::byte* dest, const std::u32string_view str)
{
	std::ranges::for_each(str, [&dest](const char32_t& ch) {
		dest = Serialize(dest, ch);
	});

	return dest;
}

const std::byte*
saga::deserializers::Deserialize(const std::byte* buffer, size_t length, std::string& output)
{
	output.reserve(length);

	for (auto it = buffer; it < buffer + length; ++it)
	{
		output.push_back(std::bit_cast<char>(*it));
	}

	return buffer + length;
}

#ifdef __cpp_lib_char8_t
const std::byte*
saga::deserializers::Deserialize(const std::byte* buffer, size_t length, std::u8string& output)
{
	output.reserve(length);

	const std::byte* it = buffer;
	while (it < buffer + length * sizeof(char8_t))
	{
		output.push_back(std::bit_cast<char>(*it));
		++it;
	}

	return it;
}
#endif // __cpp_lib_char8_t

const std::byte*
saga::deserializers::Deserialize(const std::byte* buffer, size_t length, std::wstring& output)
{
	output.reserve(length);

	const std::byte* it = buffer;
	while (it < buffer + length * sizeof(wchar_t))
	{
		wchar_t ch{};

		it = Deserialize(it, ch);

		output.push_back(ch);
	}

	return it;
}

const std::byte*
saga::deserializers::Deserialize(const std::byte* buffer, size_t length, std::u16string& output)
{
	output.reserve(length);

	const std::byte* it = buffer;
	while (it < buffer + length * sizeof(char16_t))
	{
		char16_t ch{};

		it = Deserialize(it, ch);

		output.push_back(ch);
	}

	return it;
}

const std::byte*
saga::deserializers::Deserialize(const std::byte* buffer, size_t length, std::u32string& output)
{
	output.reserve(length);

	const std::byte* it = buffer;
	while (it < buffer + length * sizeof(char32_t))
	{
		char32_t ch{};

		it = Deserialize(it, ch);

		output.push_back(ch);
	}

	return it;
}

template<typename ...Args>
std::unique_ptr<std::byte[]>
saga::Serialize(const std::tuple<Args...>& tuple)
{
	const size_t bfsize = detail::GetByteSize(tuple, std::index_sequence_for<Args...>{});

	std::unique_ptr<std::byte[]> buffer = std::make_unique<std::byte[]>(bfsize);
	Serialize(buffer.get(), tuple);
	return buffer;
}

template<typename ...Args>
std::byte*
saga::Serialize(std::byte* dest, const std::tuple<Args...>& tuple)
{
	if constexpr (0 < sizeof...(Args))
	{
		return saga::detail::Serialize(dest, tuple, std::index_sequence_for<Args...>{});
	}
	else
	{
		return dest;
	}
}

template<typename ...Args>
std::unique_ptr<std::byte[]>
saga::Serialize(std::tuple<Args...>&& tuple)
{
	const size_t bfsize = detail::GetByteSize(tuple, std::index_sequence_for<Args...>{});

	std::unique_ptr<std::byte[]> buffer = std::make_unique<std::byte[]>(bfsize);
	Serialize(buffer.get(), std::move(tuple));
	return buffer;
}

template<typename ...Args>
std::byte*
saga::Serialize(std::byte* dest, std::tuple<Args...>&& tuple)
{
	if constexpr (0 < sizeof...(Args))
	{
		return saga::detail::Serialize(dest, std::move(tuple), std::index_sequence_for<Args...>{});
	}
	else
	{
		return dest;
	}
}

template<typename ...Args> requires (1 < sizeof...(Args))
	std::unique_ptr<std::byte[]>
	saga::Serializes(Args && ...args)
{
	return Serialize(std::forward_as_tuple(std::forward<Args>(args)...));
}

template<typename ...Args> requires (0 < sizeof...(Args))
	std::byte*
	saga::Serializes(std::byte* dest, Args && ...args)
{
	return saga::detail::Serialize(dest, std::forward_as_tuple(std::forward<Args>(args)...), std::index_sequence_for<Args...>{});
}

#endif // !SAGAFRAMEWORK_UTIL_SERIALIZER_H
