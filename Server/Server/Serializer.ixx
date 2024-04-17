module;
#define ICONER_SERIALIZER_NODISCARD [[nodiscard("The serialized buffer has been lost!")]]
#include <cstdint>
#include <cmath>
#include <stdexcept>
#include <bit>
#include <tuple>
#include <utility>
#include <algorithm>

export module Iconer.Utility.Serializer;
import Iconer.Utility.Constraints;
import Iconer.Utility.Byte;
import Iconer.Utility.File;
export import <memory>;
export import <string>;
export import <string_view>;

namespace iconer::util::inline serializers
{
	/// <summary>Transfer a boolean value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const bool& value)
	{
		dest[0] = std::bit_cast<std::byte>(value);
		return dest + 1;
	}

	/// <summary>Allocate a byte buffer for a boolean value</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const bool& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(bool));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer a character to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const char& value)
	{
		dest[0] = std::bit_cast<std::byte>(value);
		return dest + 1;
	}

	/// <summary>Allocate a byte buffer for a character</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr std::unique_ptr<std::byte[]> Serialize(const char& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(char));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer an unsigned character to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const unsigned char& value)
	{
		dest[0] = std::bit_cast<std::byte>(value);
		return dest + 1;
	}

	/// <summary>Allocate a byte buffer for an unsigned character</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const unsigned char& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(unsigned char));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer an utf-8 character to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const char8_t& value)
	{
		dest[0] = std::bit_cast<std::byte>(value);
		return dest + 1;
	}

	/// <summary>Allocate a byte buffer for an utf-8 character</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const char8_t& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(char8_t));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer a 16-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const std::int16_t& value)
	{
		const auto longer = static_cast<std::int32_t>(value);
		dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
		dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));

		return dest + 2;
	}

	/// <summary>Allocate a byte buffer for a 16-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const std::int16_t& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(std::int16_t));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer an unsigned 16-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const std::uint16_t& value)
	{
		const auto longer = static_cast<std::uint32_t>(value);
		dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
		dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));

		return dest + 2;
	}

	/// <summary>Allocate a byte buffer for an unsigned 16-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const std::uint16_t& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(std::uint16_t));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer an utf-16 character to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const char16_t& value)
	{
		const auto longer = static_cast<std::uint32_t>(value);
		dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
		dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));

		return dest + 2;
	}

	/// <summary>Allocate a byte buffer for an utf-16 character</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const char16_t& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(char16_t));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer a wide character to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const wchar_t& value)
	{
		const auto longer = static_cast<std::uint32_t>(value);
		dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
		dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));

		return dest + 2;
	}

	/// <summary>Allocate a byte buffer for a wide character</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const wchar_t& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(wchar_t));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer a 32-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const std::int32_t& value)
	{
		const auto longer = static_cast<std::int64_t>(value);
		dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
		dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));
		dest[2] = std::bit_cast<std::byte>(RShift(longer, 2));
		dest[3] = std::bit_cast<std::byte>(RShift(longer, 3));

		return dest + 4;
	}

	/// <summary>Allocate a byte buffer for a 32-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const std::int32_t& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(std::int32_t));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer an unsigned 32-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const std::uint32_t& value)
	{
		const auto longer = static_cast<std::uint64_t>(value);
		dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
		dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));
		dest[2] = std::bit_cast<std::byte>(RShift(longer, 2));
		dest[3] = std::bit_cast<std::byte>(RShift(longer, 3));

		return dest + 4;
	}

	/// <summary>Allocate a byte buffer for  an unsigned 32-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const std::uint32_t& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(std::uint32_t));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer a 32-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const long& value)
	{
		const auto longer = static_cast<std::int64_t>(value);
		dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
		dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));
		dest[2] = std::bit_cast<std::byte>(RShift(longer, 2));
		dest[3] = std::bit_cast<std::byte>(RShift(longer, 3));

		return dest + 4;
	}

	/// <summary>Allocate a byte buffer for a 32-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const long& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(long));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer an unsigned 32-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const unsigned long& value)
	{
		const auto longer = static_cast<std::uint64_t>(value);
		dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
		dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));
		dest[2] = std::bit_cast<std::byte>(RShift(longer, 2));
		dest[3] = std::bit_cast<std::byte>(RShift(longer, 3));

		return dest + 4;
	}

	/// <summary>Allocate a byte buffer for an unsigned 32-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const unsigned long& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(unsigned long));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer an utf-32 character to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const char32_t& value)
	{
		const auto longer = static_cast<std::uint64_t>(value);
		dest[0] = std::bit_cast<std::byte>(RShift(longer, 0));
		dest[1] = std::bit_cast<std::byte>(RShift(longer, 1));
		dest[2] = std::bit_cast<std::byte>(RShift(longer, 2));
		dest[3] = std::bit_cast<std::byte>(RShift(longer, 3));

		return dest + 4;
	}

	/// <summary>Allocate a byte buffer for an utf-32 character</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const char32_t& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(char32_t));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer a 64-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const std::int64_t& value)
	{
		std::byte* it = dest;
		for (size_t i = 0; i < 8; ++i)
		{
			*(it++) = std::bit_cast<std::byte>(RShift(value, i));
		}

		return it;
	}

	/// <summary>Allocate a byte buffer for a 64-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const std::int64_t& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(std::int64_t));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer an unsigned 64-bit integer value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const std::uint64_t& value)
	{
		std::byte* it = dest;
		for (size_t i = 0; i < 8; ++i)
		{
			*(it++) = std::bit_cast<std::byte>(RShift(value, i));
		}

		return it;
	}

	/// <summary>Allocate a byte buffer for an unsigned 64-bit integer value</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const std::uint64_t& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(std::uint64_t));
		Serialize(buffer.get(), value);
		return std::move(buffer);
	}

	/// <summary>Transfer a 32-bit floating point value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const std::float_t& value)
	{
		return Serialize(dest, std::bit_cast<std::int32_t>(value));
	}

	/// <summary>Allocate a byte buffer for a 32-bit floating point value</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const std::float_t& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(std::int32_t));
		Serialize(buffer.get(), std::bit_cast<std::int32_t>(value));
		return std::move(buffer);
	}

	/// <summary>Transfer a 32-bit floating point value to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const std::double_t& value)
	{
		return Serialize(dest, std::bit_cast<std::int64_t>(value));
	}

	/// <summary>Allocate a byte buffer for a 64-bit floating point value</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const std::double_t& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(std::int64_t));
		Serialize(buffer.get(), std::bit_cast<std::int64_t>(value));
		return std::move(buffer);
	}

	/// <summary>Transfer a string to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const std::string_view str)
	{
		return std::ranges::transform(str.cbegin(), str.cend(), dest
			, [](const char& ch) noexcept
			{
				return std::bit_cast<std::byte>(ch);
			}
		).out;
	}

	/// <summary>Allocate a byte buffer for a string</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const std::string_view str)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(char) * str.length());
		Serialize(buffer.get(), str);
		return std::move(buffer);
	}

	/// <summary>Transfer a wide string to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const std::wstring_view str)
	{
		std::ranges::for_each(str, [&dest](const wchar_t& ch)
			{
				dest = Serialize(dest, ch);
			}
		);

		return dest;
	}

	/// <summary>Allocate a byte buffer for a wide string</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const std::wstring_view str)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(wchar_t) * str.length());
		Serialize(buffer.get(), str);
		return std::move(buffer);
	}

	/// <summary>Transfer an utf-8 string to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const std::u8string_view str)
	{
		return std::ranges::transform(str.cbegin(), str.cend(), dest
			, [](const char8_t& ch)
			{
				return std::bit_cast<std::byte>(ch);
			}
		).out;
	}

	/// <summary>Allocate a byte buffer for an utf-8 string</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const std::u8string_view str)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(char8_t) * str.length());
		Serialize(buffer.get(), str);
		return std::move(buffer);
	}

	/// <summary>Transfer a wide string to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const std::u16string_view str)
	{
		std::ranges::for_each(str, [&dest](const char16_t& ch)
			{
				dest = Serialize(dest, ch);
			}
		);

		return dest;
	}

	/// <summary>Allocate a byte buffer for a string</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const std::u16string_view str)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(char16_t) * str.length());
		Serialize(buffer.get(), str);
		return std::move(buffer);
	}

	/// <summary>Transfer a wide string to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const std::u32string_view str)
	{
		std::ranges::for_each(str, [&dest](const char32_t& ch)
			{
				dest = Serialize(dest, ch);
			}
		);

		return dest;
	}

	/// <summary>Allocate a byte buffer for a enumeration</summary>
	/// <exception cref="std::bad_alloc"/>
	export ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const hard_enumerations auto& value)
	{
		auto buffer = std::make_unique<std::byte[]>(sizeof(value));
		Serialize(buffer.get(), std::to_underlying(value));
		return std::move(buffer);
	}

	/// <summary>Transfer a enumeration to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr std::byte* Serialize(std::byte* dest, const hard_enumerations auto& value)
	{
		return Serialize(dest, std::to_underlying(value));
	}
}

namespace
{
	template<typename Tuple, size_t... Indices>
	constexpr std::byte* Serialize(std::byte* dest, Tuple&& tuple, std::index_sequence<Indices...>)
	{
		std::byte* it = dest;

		((it = iconer::util::Serialize(it, std::get<Indices>(std::forward<Tuple>(tuple)))), ...);

		return it;
	}
}

namespace iconer::util::inline serializers
{
	/// <summary>Allocate a byte buffer for a copied tuple</summary>
	/// <exception cref="std::bad_alloc"/>
	export template<typename... Args> ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(const std::tuple<Args...>& tuple)
	{
		const size_t bfsize = GetByteSize(tuple, std::index_sequence_for<Args...>{});

		std::unique_ptr<std::byte[]> buffer = std::make_unique<std::byte[]>(bfsize);
		Serialize(buffer.get(), tuple);
		return std::move(buffer);
	}

	/// <summary>Transfer a copied tuple to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	export template<typename... Args> constexpr std::byte* Serialize(std::byte* dest, const std::tuple<Args...>& tuple)
	{
		if constexpr (0 < sizeof...(Args))
		{
			return ::Serialize(dest, tuple, std::index_sequence_for<Args...>{});
		}
		else
		{
			return dest;
		}
	}

	/// <summary>Allocate a byte buffer for a moved tuple</summary>
	/// <exception cref="std::bad_alloc"/>
	export template<typename... Args> ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serialize(std::tuple<Args...>&& tuple)
	{
		const size_t bfsize = GetByteSize(tuple, std::index_sequence_for<Args...>{});

		std::unique_ptr<std::byte[]> buffer = std::make_unique<std::byte[]>(bfsize);
		Serialize(buffer.get(), std::move(tuple));
		return std::move(buffer);
	}

	/// <summary>Transfer a moved tuple to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	export template<typename... Args> constexpr std::byte* Serialize(std::byte* dest, std::tuple<Args...>&& tuple)
	{
		if constexpr (0 < sizeof...(Args))
		{
			return ::Serialize(dest, std::move(tuple), std::index_sequence_for<Args...>{});
		}
		else
		{
			return dest;
		}
	}

	/// <summary>Allocate a byte buffer of multiple values</summary>
	/// <exception cref="std::bad_alloc"/>
	export template<typename... Args> ICONER_SERIALIZER_NODISCARD constexpr
		std::unique_ptr<std::byte[]> Serializes(Args&&... args)
	{
		return Serialize(std::forward_as_tuple(std::forward<Args>(args)...));
	}

	/// <summary>Transfer arguments to the byte buffer</summary>
	/// <returns>last buffer pointer after dest</returns>
	export template<typename... Args> constexpr std::byte* Serializes(std::byte* dest, Args&&... args)
	{
		return ::Serialize(dest, std::forward_as_tuple(std::forward<Args>(args)...), std::index_sequence_for<Args...>{});
	}
}

namespace iconer::util::inline deserializers
{
	/// <summary>Read a boolean value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, bool& output)
	{
		output = std::bit_cast<char>(buffer[0]);

		return buffer + sizeof(bool);
	}

	/// <summary>Read a character from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, char& output)
	{
		output = std::bit_cast<char>(buffer[0]);

		return buffer + sizeof(char);
	}

	/// <summary>Read a signed character from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, signed char& output)
	{
		output = std::bit_cast<signed char>(buffer[0]);

		return buffer + sizeof(signed char);
	}

	/// <summary>Read an unsigned character from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, unsigned char& output)
	{
		output = std::bit_cast<unsigned char>(buffer[0]);

		return buffer + sizeof(unsigned char);
	}

	/// <summary>Read an utf-8 character from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, char8_t& output)
	{
		output = std::bit_cast<char8_t>(buffer[0]);

		return buffer + sizeof(char8_t);
	}

	/// <summary>Read a 16-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, std::int16_t& output)
	{
		const std::uint32_t mid = (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<std::int16_t>(mid);

		return buffer + sizeof(std::int16_t);
	}

	/// <summary>Read an unsigned 16-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, std::uint16_t& output)
	{
		const std::uint32_t mid = (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<std::uint16_t>(mid);

		return buffer + sizeof(std::uint16_t);
	}

	/// <summary>Read a wide character from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, wchar_t& output)
	{
		const std::uint32_t mid = (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<wchar_t>(mid);

		return buffer + sizeof(wchar_t);
	}

	/// <summary>Read an utf-16 character from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, char16_t& output)
	{
		const std::uint32_t mid = (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<char16_t>(mid);

		return buffer + sizeof(char16_t);
	}

	/// <summary>Read a 32-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, std::int32_t& output)
	{
		const std::uint64_t mid = (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[3])) << 24U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[2])) << 16U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<std::int32_t>(mid);

		return buffer + sizeof(std::int32_t);
	}

	/// <summary>Read an unsigned 32-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, std::uint32_t& output)
	{
		const std::uint64_t mid = (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[3])) << 24U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[2])) << 16U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<std::uint32_t>(mid);

		return buffer + sizeof(std::uint32_t);
	}

	/// <summary>Read a 32-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, long& output)
	{
		const std::uint64_t mid = (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[3])) << 24U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[2])) << 16U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<long>(mid);

		return buffer + sizeof(long);
	}

	/// <summary>Read an unsigned 32-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, unsigned long& output)
	{
		const std::uint64_t mid = (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[3])) << 24U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[2])) << 16U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<unsigned long>(mid);

		return buffer + sizeof(unsigned long);
	}

	/// <summary>Read an utf-32 character from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, char32_t& output)
	{
		const std::uint64_t mid = (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[3])) << 24U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[2])) << 16U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<char32_t>(mid);

		return buffer + sizeof(char32_t);
	}

	/// <summary>Read a 64-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, std::int64_t& output)
	{
		output = static_cast<std::int64_t>((static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[7])) << 56U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[6])) << 48U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[5])) << 40U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[4])) << 32U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[3])) << 24U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[2])) << 16U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[0])));

		return buffer + sizeof(std::int64_t);
	}

	/// <summary>Read an unsigned 64-bit integer value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, std::uint64_t& output)
	{
		output = (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[7])) << 56U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[6])) << 48U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[5])) << 40U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[4])) << 32U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[3])) << 24U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[2])) << 16U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[0]));

		return buffer + sizeof(std::uint64_t);
	}

	/// <summary>Read a 32-bit floating point value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, std::float_t& output)
	{
		std::int32_t mid{};
		auto it = Deserialize(buffer, mid);

		output = std::bit_cast<std::float_t>(mid);

		return it;
	}

	/// <summary>Read a 64-bit floating point value from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, std::double_t& output)
	{
		std::int64_t mid{};
		auto it = Deserialize(buffer, mid);

		output = std::bit_cast<std::double_t>(mid);

		return it;
	}

	/// <summary>Read a string from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, size_t length, std::string& output)
	{
		output.reserve(length);

		for (auto it = buffer; it < buffer + length; ++it)
		{
			output.push_back(std::bit_cast<char>(*it));
		}

		return buffer + length;
	}

	/// <summary>Read a utf-8 string from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, size_t length, std::u8string& output)
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

	/// <summary>Read a wide string from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, size_t length, std::wstring& output)
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

	/// <summary>Read a utf-16 string from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, size_t length, std::u16string& output)
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

	/// <summary>Read a utf-32 string from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export constexpr const std::byte* Deserialize(const std::byte* buffer, size_t length, std::u32string& output)
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

	/// <summary>Read a array from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export template<size_t Length> constexpr const std::byte* Deserialize(const std::byte* buffer, char(&output)[Length])
	{
		const std::byte* it = buffer;
		char* out = output;

		while (it < buffer + Length * sizeof(char))
		{
			*out = std::bit_cast<char>(*it);

			++it; ++out;
		}

		return it;
	}

	/// <summary>Read a array from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export template<size_t Length> constexpr const std::byte* Deserialize(const std::byte* buffer, char8_t(&output)[Length])
	{
		const std::byte* it = buffer;
		char8_t* out = output;

		while (it < buffer + Length * sizeof(char8_t))
		{
			*out = std::bit_cast<char8_t>(*it);

			++it;

			if (++out == std::cend(output))
			{
				break;
			}
		}

		return it;
	}

	/// <summary>Read a array from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export template<size_t Length> constexpr const std::byte* Deserialize(const std::byte* buffer, wchar_t(&output)[Length])
	{
		const std::byte* it = buffer;
		wchar_t* out = output;

		while (it < buffer + Length * sizeof(wchar_t))
		{
			it = Deserialize(it, *out);

			if (++out == std::cend(output))
			{
				break;
			}
		}

		return it;
	}

	/// <summary>Read a array from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export template<size_t Length> constexpr const std::byte* Deserialize(const std::byte* buffer, char16_t(&output)[Length])
	{
		const std::byte* it = buffer;
		char16_t* out = output;

		while (it < buffer + Length * sizeof(char16_t))
		{
			it = Deserialize(it, *out);

			if (++out == std::cend(output))
			{
				break;
			}
		}

		return it;
	}

	/// <summary>Read a array from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export template<size_t Length> constexpr const std::byte* Deserialize(const std::byte* buffer, char32_t(&output)[Length])
	{
		const std::byte* it = buffer;
		char32_t* out = output;

		while (it < buffer + Length * sizeof(char32_t))
		{
			it = Deserialize(it, *out);

			if (++out == std::cend(output))
			{
				break;
			}
		}

		return it;
	}

	/// <summary>Read a enumeration from the byte buffer</summary>
	/// <returns>last buffer pointer after read dest</returns>
	/// <exception cref="std::out_of_range"/>
	export template<hard_enumerations T> constexpr const std::byte* Deserialize(const std::byte* buffer, T& output)
	{
		std::underlying_type_t<T> result{};
		auto it = Deserialize(buffer, result);
		output = static_cast<T>(result);

		return it;
	}
}

module :private;

namespace iconer::util::test
{
	constexpr std::byte aaa1()
	{
		std::byte buffer[2]{};

		Serialize(buffer, true);

		return buffer[0];
	}

	constexpr std::byte aaa2()
	{
		std::byte buffer[2]{};

		Serialize(buffer, u8'a');

		return buffer[0];
	}

	constexpr std::byte aaa3()
	{
		std::byte buffer[2]{};

		Serialize(buffer, '7');

		return buffer[0];
	}

	constexpr std::byte aaa4()
	{
		std::byte buffer[4]{};

		Serialize(buffer, 10000000);

		return buffer[2];
	}

	constexpr auto aaa5()
	{
		std::byte buffer[2]{};

		// 'o' == 111
		Serialize(buffer, L'o');

		return buffer[0];
	}

	constexpr auto aaa6()
	{
		std::byte buffer[2]{};

		constexpr auto cvt = L'가';
		Serialize(buffer, L'가');

		return buffer[1];
	}

	constexpr auto aaa7()
	{
		std::byte buffer[2]{};

		constexpr auto cvt = u'가';
		Serialize(buffer, u'가');

		return buffer[1];
	}

	constexpr auto aaa8()
	{
		std::byte buffer[4]{};

		constexpr auto cvt = U'가';
		Serialize(buffer, U'가');

		return buffer[1];
	}

	constexpr std::int16_t aaa9()
	{
		constexpr std::byte test_int16[2]{ std::byte{1}, std::byte{0} };

		std::int16_t result;
		Deserialize(test_int16, result);

		return result;
	}

	constexpr std::uint16_t aaa10()
	{
		constexpr std::uint16_t test_int16 = 500;

		std::byte buffer[2]{};
		Serialize(buffer, test_int16);

		std::uint16_t result;
		Deserialize(buffer, result);

		return result;
	}

	constexpr char16_t aaa11()
	{
		constexpr auto test_char16 = u'하';

		std::byte buffer[2]{};
		Serialize(buffer, test_char16);

		char16_t result;
		Deserialize(buffer, result);

		return result;
	}

	constexpr std::int32_t aaa12()
	{
		constexpr std::int32_t test_int32 = 71893451;

		std::byte buffer[4]{};
		Serialize(buffer, test_int32);

		std::int32_t result;
		Deserialize(buffer, result);

		return result;
	}

	constexpr std::int64_t aaa13()
	{
		constexpr std::int64_t test_value = 27012391694894271;

		std::byte buffer[8]{};
		Serialize(buffer, test_value);

		std::int64_t result;
		Deserialize(buffer, result);

		return result;
	}

	constexpr std::uint64_t aaa14()
	{
		constexpr std::uint64_t test_value = 3602986718492;

		std::byte buffer[8]{};
		Serialize(buffer, test_value);

		std::uint64_t result;
		Deserialize(buffer, result);

		return result;
	}

	constexpr std::float_t aaa15()
	{
		constexpr std::float_t test_float = 1700345.3139513f;

		std::byte buffer[4]{};
		Serialize(buffer, test_float);

		std::float_t result;
		Deserialize(buffer, result);

		return result;
	}

	constexpr std::double_t aaa16()
	{
		constexpr std::double_t test_double = 6049148692014.946781231024;

		std::byte buffer[8]{};
		Serialize(buffer, test_double);

		std::double_t result;
		Deserialize(buffer, result);

		return result;
	}

	constexpr auto aaa17()
	{
		constexpr std::string_view test_str = "가나다라마바사아자차카타파하";
		std::byte buffer[test_str.length()]{};

		//std::byte buffer[200]{};
		Serialize(buffer, test_str);

		char result[test_str.length()]{};
		Deserialize(buffer, result);

		//std::string result{};
		//Deserialize(buffer, test_str.length(), result);

		bool check = true;
		for (size_t i = 0; i < test_str.length(); ++i)
		{
			if (buffer[i] != static_cast<std::byte>(static_cast<unsigned char>(result[i])))
			{
				check = false;
			}
		}

		return check;
	}

	constexpr auto aaa18()
	{
		constexpr std::string_view test_str = "가나다라마바사아자차카타파하";

		std::byte buffer[test_str.length() * sizeof(char) + 1]{};
		Serialize(buffer, test_str);

		std::string result{};
		Deserialize(buffer, test_str.length(), result);

		return result.at(0);
	}

	constexpr auto aaa19()
	{
		constexpr std::wstring_view test_str = L"가aa나다라마바사아자차카타파하";

		std::byte buffer[test_str.size() * sizeof(wchar_t) + 1]{};
		Serialize(buffer, test_str);

		std::wstring result{};
		Deserialize(buffer, test_str.length(), result);

		constexpr size_t target_offset = 15;
		constexpr wchar_t target = test_str.at(target_offset);
		return result.at(target_offset);
	}

	constexpr auto aaa20()
	{
		constexpr std::u32string_view test_str = U"가aa나다라마바사아자차카타파하";

		std::byte buffer[test_str.size() * sizeof(char32_t) + 1]{};
		Serialize(buffer, test_str);

		std::u32string result{};
		Deserialize(buffer, test_str.length(), result);

		constexpr size_t target_offset = 14;
		constexpr char32_t target = test_str.at(target_offset);
		return result.at(target_offset);
	}

	constexpr void testments()
	{
		constexpr auto v_aaa1 = aaa1();
		constexpr auto v_aaa2 = aaa2();
		constexpr auto v_aaa3 = aaa3();
		constexpr auto v_aaa4 = aaa4();
		constexpr auto v_aaa5 = aaa5();
		constexpr auto v_aaa6 = aaa6();
		constexpr auto v_aaa7 = aaa7();
		constexpr auto v_aaa8 = aaa8();
		constexpr auto v_aaa9 = aaa9();
		constexpr auto v_aaa10 = aaa10();
		constexpr auto v_aaa11 = aaa11();
		constexpr auto v_aaa12 = aaa12();
		constexpr auto v_aaa13 = aaa13();
		constexpr auto v_aaa14 = aaa14();
		constexpr auto v_aaa15 = aaa15();
		constexpr auto v_aaa16 = aaa16();
		constexpr auto v_aaa17 = aaa17();
		static_assert(v_aaa17);

		//constexpr auto v_aaa18 = aaa18();
		//constexpr auto v_aaa19 = aaa19();
		//constexpr auto v_aaa20 = aaa20();
	}
}
