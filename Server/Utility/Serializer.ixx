module;
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <bit>
#include <memory>

export module iconer.Utility.Serializer;
import Iconer.Utility.TypeTraits;
import <string_view>;
import <string>;
import <vector>;
import <array>;
import <ranges>;

export namespace iconer::util
{
	template<typename T>
	constexpr std::int16_t GetByteSize(const T&) noexcept;

	template<trivials T>
	[[nodiscard]]
	constexpr std::int16_t GetByteSize(const T&) noexcept
	{
		return sizeof(T);
	}

	template<trivials T, size_t Len>
	[[nodiscard]]
	constexpr std::int16_t GetByteSize(const T(&)[Len]) noexcept
	{
		return static_cast<std::int16_t>(sizeof(T) * Len);
	}

	template<trivials T, typename Traits>
	[[nodiscard]]
	constexpr std::int16_t GetByteSize(const std::basic_string<T, Traits>& value) noexcept
	{
		return static_cast<std::int16_t>(sizeof(T) * value.length());
	}

	template<trivials T, typename Traits>
	[[nodiscard]]
	constexpr std::int16_t GetByteSize(std::basic_string_view<T, Traits> value) noexcept
	{
		return static_cast<std::int16_t>(sizeof(T) * value.length());
	}

	template<trivials T, size_t Len>
	[[nodiscard]]
	constexpr std::int16_t GetByteSize(const std::array<T, Len>&) noexcept
	{
		return static_cast<std::int16_t>(sizeof(T) * Len);
	}

	template<trivials T>
	[[nodiscard]]
	constexpr std::int16_t GetByteSize(const std::vector<T>& value) noexcept
	{
		return static_cast<std::int16_t>(sizeof(T) * value.size());
	}

	template<std::ranges::contiguous_range R>
	[[nodiscard]]
	constexpr std::int16_t GetByteSize(const R& range) noexcept
	{
		return static_cast<std::int16_t>(sizeof(std::ranges::range_value_t<R>) * std::ranges::size(range));
	}

	template<typename... Ts>
	[[nodiscard]]
	constexpr std::int16_t GetTotalByteSize(const Ts&... params) noexcept
	{
		std::int16_t result{};

		((result += GetByteSize(params)), ...);

		return result;
	}

	template<typename T>
	std::byte* Serialize(std::byte* buffer, const T& value);

	template<integrals T>
	constexpr std::byte* Serialize(std::byte* buffer, const T& value)
	{
		if (std::is_constant_evaluated())
		{
			constexpr size_t size = sizeof(T);
			constexpr std::uint8_t byte_fix = 0xFF;

			switch (size)
			{
			case 1:
			{
				*buffer = static_cast<std::byte>(static_cast<std::uint8_t>(value));
			}
			break;

			case 2:
			{
				const auto mid = static_cast<std::uint16_t>(value);
				//const auto mid = static_cast<std::uint16_t>(20);

				constexpr std::uint16_t lhs_fix = 0xFF00;
				constexpr std::uint16_t rhs_fix = 0x00FF;

				const std::uint16_t lhs = mid & lhs_fix;
				const std::uint16_t rhs = mid & rhs_fix;
				const std::uint8_t post_lhs = static_cast<std::uint8_t>(lhs) & byte_fix;
				const std::uint8_t post_rhs = static_cast<std::uint8_t>(rhs) & byte_fix;

				buffer[0] = static_cast<std::byte>(post_lhs);
				buffer[1] = static_cast<std::byte>(post_rhs);
			}
			break;

			case 4:
			{
				const auto mid = static_cast<std::uint32_t>(value);
				//const auto mid = static_cast<std::uint32_t>(20);

				constexpr std::uint32_t llhs_fix = 0xFF000000;
				constexpr std::uint32_t lrhs_fix = 0x00FF0000;
				constexpr std::uint32_t rlhs_fix = 0x0000FF00;
				constexpr std::uint32_t rrhs_fix = 0x000000FF;

				const std::uint32_t llhs = mid & llhs_fix;
				const std::uint32_t lrhs = mid & lrhs_fix;
				const std::uint32_t rlhs = mid & rlhs_fix;
				const std::uint32_t rrhs = mid & rrhs_fix;

				const auto post_llhs = static_cast<std::uint8_t>(static_cast<std::uint8_t>(llhs) & byte_fix);
				const auto post_lrhs = static_cast<std::uint8_t>(static_cast<std::uint8_t>(lrhs) & byte_fix);
				const auto post_rlhs = static_cast<std::uint8_t>(static_cast<std::uint8_t>(rlhs) & byte_fix);
				const auto post_rrhs = static_cast<std::uint8_t>(static_cast<std::uint8_t>(rrhs) & byte_fix);

				buffer[0] = static_cast<std::byte>(post_llhs);
				buffer[1] = static_cast<std::byte>(post_lrhs);
				buffer[2] = static_cast<std::byte>(post_rlhs);
				buffer[3] = static_cast<std::byte>(post_rrhs);
			}
			break;

			case 8:
			{
				const auto mid = static_cast<std::uint64_t>(value);
				//const auto mid = static_cast<std::uint64_t>(20);

				constexpr std::uint64_t f_llhs_fix = 0xFF00000000000000;
				constexpr std::uint64_t f_lrhs_fix = 0x00FF000000000000;
				constexpr std::uint64_t f_rlhs_fix = 0x0000FF0000000000;
				constexpr std::uint64_t f_rrhs_fix = 0x000000FF00000000;

				constexpr std::uint64_t n_llhs_fix = 0x00000000FF000000;
				constexpr std::uint64_t n_lrhs_fix = 0x0000000000FF0000;
				constexpr std::uint64_t n_rlhs_fix = 0x000000000000FF00;
				constexpr std::uint64_t n_rrhs_fix = 0x00000000000000FF;

				const std::uint64_t f_llhs = mid & f_llhs_fix;
				const std::uint64_t f_lrhs = mid & f_lrhs_fix;
				const std::uint64_t f_rlhs = mid & f_rlhs_fix;
				const std::uint64_t f_rrhs = mid & f_rrhs_fix;

				const std::uint64_t n_llhs = mid & n_llhs_fix;
				const std::uint64_t n_lrhs = mid & n_lrhs_fix;
				const std::uint64_t n_rlhs = mid & n_rlhs_fix;
				const std::uint64_t n_rrhs = mid & n_rrhs_fix;

				const std::uint8_t post_far_llhs = static_cast<std::uint8_t>(f_llhs) & byte_fix;
				const std::uint8_t post_far_lrhs = static_cast<std::uint8_t>(f_lrhs) & byte_fix;
				const std::uint8_t post_far_rlhs = static_cast<std::uint8_t>(f_rlhs) & byte_fix;
				const std::uint8_t post_far_rrhs = static_cast<std::uint8_t>(f_rrhs) & byte_fix;

				const std::uint8_t post_near_llhs = static_cast<std::uint8_t>(n_llhs) & byte_fix;
				const std::uint8_t post_near_lrhs = static_cast<std::uint8_t>(n_lrhs) & byte_fix;
				const std::uint8_t post_near_rlhs = static_cast<std::uint8_t>(n_rlhs) & byte_fix;
				const std::uint8_t post_near_rrhs = static_cast<std::uint8_t>(n_rrhs) & byte_fix;

				buffer[7] = static_cast<std::byte>(post_far_llhs);
				buffer[6] = static_cast<std::byte>(post_far_lrhs);
				buffer[5] = static_cast<std::byte>(post_far_rlhs);
				buffer[4] = static_cast<std::byte>(post_far_rrhs);
				buffer[3] = static_cast<std::byte>(post_near_llhs);
				buffer[2] = static_cast<std::byte>(post_near_lrhs);
				buffer[1] = static_cast<std::byte>(post_near_rlhs);
				buffer[0] = static_cast<std::byte>(post_near_rrhs);
			}
			break;

			case 16:
			{
				throw "Error";
			}
			break;
			}
		}
		else
		{
			const T nvalue = std::byteswap(value);

			std::memcpy(buffer, std::addressof(value), sizeof(T));
		}

		return buffer + sizeof(T);
	}

	template<floats T>
	constexpr std::byte* Serialize(std::byte* buffer, const T& value)
	{
		if constexpr (std::same_as<T, float>)
		{
			return Serialize(buffer, std::bit_cast<std::int32_t>(value));
		}
		else if constexpr (std::same_as<T, double>)
		{
			return Serialize(buffer, std::bit_cast<std::int64_t>(value));
		}
		else if constexpr (std::same_as<T, long double>)
		{
			return Serialize(buffer, std::bit_cast<std::int64_t>(value));
		}
	}

	template<classes T>
		requires trivials<T>
	std::byte* Serialize(std::byte* buffer, T&& value)
	{
		std::memcpy(buffer, std::addressof(value), sizeof(T));
		return buffer + sizeof(T);
	}

	template<enumerations T>
	constexpr std::byte* Serialize(std::byte* buffer, const T& value)
	{
		return Serialize(buffer, std::to_underlying(value));
	}

	template<trivials T, size_t Len>
	constexpr std::byte* Serialize(std::byte* buffer, const T(&array)[Len])
	{
		if (std::is_constant_evaluated())
		{
			for (auto&& element : array)
			{
				buffer = Serialize(buffer, element);
			}

			return buffer;
		}
		else
		{
			const size_t size = sizeof(T) * Len;
			std::memcpy(buffer, std::addressof(array), size);
			return static_cast<std::byte*>(buffer + size);
		}
	}

	template<trivials T, size_t Len>
	constexpr std::byte* Serialize(std::byte* buffer, T(&& array)[Len])
	{
		if (std::is_constant_evaluated())
		{
			for (auto&& element : array)
			{
				buffer = Serialize(buffer, element);
			}

			return buffer;
		}
		else
		{
			std::memcpy(buffer, std::addressof(array), sizeof(T) * Len);
			return static_cast<std::byte*>(buffer + sizeof(T) * Len);
		}
	}

	template<trivials T, typename Traits>
	constexpr std::byte* Serialize(std::byte* buffer, const std::basic_string<T, Traits>& str)
	{
		if (std::is_constant_evaluated())
		{
			for (auto&& element : str)
			{
				buffer = Serialize(buffer, element);
			}

			return buffer;
		}
		else
		{
			const size_t size = sizeof(T) * str.length();
			std::memcpy(buffer, str.data(), size);
			return static_cast<std::byte*>(buffer + size);
		}
	}

	template<trivials T, typename Traits>
	constexpr std::byte* Serialize(std::byte* buffer, std::basic_string<T, Traits>&& value)
	{
		const std::basic_string<T, Traits> str = static_cast<std::basic_string<T, Traits>&&>(value);

		return Serialize(buffer, str);
	}

	template<trivials T, typename Traits>
	constexpr std::byte* Serialize(std::byte* buffer, std::basic_string_view<T, Traits> str)
	{
		if (std::is_constant_evaluated())
		{
			for (auto&& element : str)
			{
				buffer = Serialize(buffer, element);
			}

			return buffer;

		}
		else
		{
			const size_t size = sizeof(T) * str.length();
			memcpy(buffer, str.data(), size);
			return static_cast<std::byte*>(buffer + size);
		}
	}

	template<trivials T, size_t Len>
	constexpr std::byte* Serialize(std::byte* buffer, const std::array<T, Len>& array)
	{
		if (std::is_constant_evaluated())
		{
			for (auto&& element : array)
			{
				buffer = Serialize(buffer, element);
			}

			return buffer;
		}
		else
		{
			const size_t size = sizeof(T) * Len;
			std::memcpy(buffer, std::addressof(array), size);
			return static_cast<std::byte*>(buffer + size);
		}
	}

	template<trivials T>
	constexpr std::byte* Serialize(std::byte* buffer, const std::vector<T>& cont)
	{
		if (std::is_constant_evaluated())
		{
			for (auto&& element : cont)
			{
				buffer = Serialize(buffer, element);
			}

			return buffer;
		}
		else
		{
			const size_t size = sizeof(T) * cont.size();
			std::memcpy(buffer, cont.data(), size);
			return static_cast<std::byte*>(buffer + size);
		}
	}

	template<std::ranges::contiguous_range R>
	constexpr std::byte* Serialize(std::byte* buffer, R&& range)
	{
		if (std::is_constant_evaluated())
		{
			for (auto&& element : range)
			{
				buffer = Serialize(buffer, element);
			}

			return buffer;
		}
		else
		{
			const size_t size = GetByteSize(range);
			std::memcpy(buffer, std::ranges::cdata(range), size);
			return static_cast<std::byte*>(buffer + size);
		}
	}
}
