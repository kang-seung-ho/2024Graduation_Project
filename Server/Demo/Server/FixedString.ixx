export module Iconer.Collection.FixedString;
import <compare>;
import <string>;
import <string_view>;
import <format>;

export namespace iconer::collection
{
	template <typename Char, size_t L>
	struct [[nodiscard]] basic_fixed_string final
	{
		static_assert(std::is_trivially_copyable_v<Char>, "Char must be trivially copyable.");
		static_assert(std::is_standard_layout_v<Char>, "Char must be standard layout.");
		static_assert(0 < L, "Fixed string must have at least one element.");

		using value_type = Char;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using pointer = Char*;
		using const_pointer = const Char*;
		using reference = Char&;
		using const_reference = const Char&;
		using iterator = Char*;
		using const_iterator = const Char*;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		static inline constexpr size_t Length = L;

		constexpr basic_fixed_string() noexcept = default;
		constexpr ~basic_fixed_string() noexcept = default;

		constexpr basic_fixed_string(const Char* const& buffer)
			: strBuffer()
		{
			for (size_t i = 0; i < Length; ++i)
			{
				const Char& elem = buffer[i];
				if (0 == elem)
				{
					break;
				}

				strBuffer[i] = elem;
			}
		}

		template<size_t Size>
		constexpr basic_fixed_string(const Char(&buffer)[Size]) noexcept(Size <= Length)
			: strBuffer()
		{
			for (size_t i = 0; i < Length; ++i)
			{
				const Char& elem = buffer[i];
				if (0 == elem)
				{
					break;
				}

				strBuffer[i] = elem;
			}
		}

		[[nodiscard]]
		constexpr reference at(size_type index)
		{
			if (Length <= index)
			{
				ThrowAccessError();
			}

			return strBuffer[index];
		}

		[[nodiscard]]
		constexpr const_reference at(size_type index) const
		{
			if (Length <= index)
			{
				ThrowAccessError();
			}

			return strBuffer[index];
		}

		[[nodiscard]]
		constexpr reference operator[](size_type index)
		{
			if (Length <= index)
			{
				ThrowAccessError();
			}

			return strBuffer[index];
		}

		[[nodiscard]]
		constexpr const_reference operator[](size_type index) const
		{
			if (Length <= index)
			{
				ThrowAccessError();
			}

			return strBuffer[index];
		}

		[[nodiscard]]
		constexpr pointer data() noexcept
		{
			return strBuffer;
		}

		[[nodiscard]]
		constexpr const_pointer data() const noexcept
		{
			return strBuffer;
		}

		[[nodiscard]]
		constexpr iterator begin() noexcept
		{
			return strBuffer;
		}

		[[nodiscard]]
		constexpr iterator end() noexcept
		{
			return strBuffer + Length;
		}

		[[nodiscard]]
		constexpr const_iterator begin() const noexcept
		{
			return strBuffer;
		}

		[[nodiscard]]
		constexpr const_iterator end() const noexcept
		{
			return strBuffer + Length;
		}

		[[nodiscard]]
		constexpr const_iterator cbegin() const noexcept
		{
			return strBuffer;
		}

		[[nodiscard]]
		constexpr const_iterator cend() const noexcept
		{
			return strBuffer + Length;
		}

		[[nodiscard]]
		constexpr size_type size() const noexcept
		{
			return Length;
		}

		[[nodiscard]]
		constexpr difference_type ssize() const noexcept
		{
			return static_cast<difference_type>(Length);
		}

		template<typename Traits = std::char_traits<Char>>
		[[nodiscard]]
		constexpr std::basic_string_view<Char, Traits> view() const noexcept
		{
			return std::basic_string_view<Char, Traits>(strBuffer, Length);
		}

		template<typename Traits = std::char_traits<Char>>
		explicit constexpr operator std::basic_string_view<Char, Traits>() const noexcept
		{
			return std::basic_string_view<Char, Traits>(strBuffer, Length);
		}

		[[nodiscard]]
		constexpr bool operator==(const basic_fixed_string&) const noexcept = default;
		[[nodiscard]]
		constexpr std::strong_ordering operator<=>(const basic_fixed_string&) const noexcept = default;

	private:
		[[noreturn]]
		static void ThrowAccessError()
		{
			throw std::out_of_range{ "Invalid basic_fixed_string<T, Length> subscript!" };
		}

		Char strBuffer[Length];
	};

	template <typename Char, size_t Length>
	basic_fixed_string(const Char(&str)[Length]) -> basic_fixed_string<Char, Length>;

	template <size_t Length>
	using fixed_string = basic_fixed_string<char, Length>;

	template <size_t Length>
	using fixed_u8string = basic_fixed_string<char8_t, Length>;

	template <size_t Length>
	using fixed_u16string = basic_fixed_string<char16_t, Length>;

	template <size_t Length>
	using fixed_u32string = basic_fixed_string<char32_t, Length>;

	template <size_t Length>
	using fixed_wstring = basic_fixed_string<wchar_t, Length>;
}

export namespace std
{
	template <typename Char, size_t Length, typename Traits = std::char_traits<char>>
	[[nodiscard]]
	string to_string(const iconer::collection::basic_fixed_string<Char, Length>& str)
	{
		return string{ static_cast<basic_string_view<char, Traits>>(str) };
	}

	template <typename Char, size_t Length, typename Traits = std::char_traits<char>>
	[[nodiscard]]
	string to_string(iconer::collection::basic_fixed_string<Char, Length>&& str)
	{
		return string{ static_cast<basic_string_view<char, Traits>>(str) };
	}
}

export template<typename Char, size_t Length>
struct std::formatter<iconer::collection::basic_fixed_string<Char, Length>, char>
{
	static constexpr
		format_parse_context::iterator
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
		format(const iconer::collection::basic_fixed_string<Char, Length>& str, format_context& context)
	{
		return format_to(context.out(), "{}", reinterpret_cast<const char*>(str.data()));
	}

	static format_context::iterator
		format(iconer::collection::basic_fixed_string<Char, Length>&& str, format_context& context)
	{
		return format_to(context.out(), "{}", reinterpret_cast<const char*>(str.data()));
	}
};

export template<typename Char, size_t Length>
struct std::formatter<iconer::collection::basic_fixed_string<Char, Length>, wchar_t>
{
	static constexpr
		wformat_parse_context::iterator
		parse(wformat_parse_context& context)
	{
		auto it = context.begin();
		const auto end = context.end();
		if (it == end or *it != L'{')
		{
			throw std::format_error{ "Invalid format string." };
		}

		++it;
		if (it == end or *it != L'}')
		{
			throw std::format_error{ "Missing '}' in format string." };
		}

		return it;
	}

	static wformat_context::iterator
		format(const iconer::collection::basic_fixed_string<Char, Length>& str, wformat_context& context)
	{
		return format_to(context.out(), L"{}", reinterpret_cast<const wchar_t*>(str.data()));
	}

	static wformat_context::iterator
		format(iconer::collection::basic_fixed_string<Char, Length>&& str, wformat_context& context)
	{
		return format_to(context.out(), L"{}", reinterpret_cast<const wchar_t*>(str.data()));
	}
};
