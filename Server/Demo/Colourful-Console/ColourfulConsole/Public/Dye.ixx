export module Iconer.Utility.ColourfulConsole.Dye;
export import Iconer.Utility.ColourfulConsole.Colour;
export import Iconer.Utility.ColourfulConsole.Palette;
import <type_traits>;
import <concepts>;
import <format>;

export namespace iconer::util::cfc
{
	template<typename T>
	class Dye
	{
	public:
		static_assert(not std::is_reference_v<T>);

		constexpr Dye() noexcept(std::is_nothrow_default_constructible_v<T>) = default;
		constexpr ~Dye() noexcept(std::is_nothrow_destructible_v<T>) = default;

		constexpr Dye(const T& value, const Colour& text_colour)
			noexcept(std::is_nothrow_copy_constructible_v<T>)
			: myData(value), myPalette{ text_colour, colors::Black }
		{
			static_assert(std::copy_constructible<T>);
		}

		constexpr Dye(const T& value, Colour&& text_colour)
			noexcept(std::is_nothrow_copy_constructible_v<T>)
			: myData(value), myPalette{ std::move(text_colour), colors::Black }
		{
			static_assert(std::copy_constructible<T>);
		}

		constexpr Dye(const T& value, const Palette& palette)
			noexcept(std::is_nothrow_copy_constructible_v<T>)
			: myData(value), myPalette(palette)
		{
			static_assert(std::copy_constructible<T>);
		}

		constexpr Dye(const T& value, Palette&& palette)
			noexcept(std::is_nothrow_copy_constructible_v<T>)
			: myData(value), myPalette(std::move(palette))
		{
			static_assert(std::copy_constructible<T>);
		}

		constexpr Dye(T&& value, const Colour& text_colour)
			noexcept(std::is_nothrow_move_constructible_v<T>)
			: myData(std::move(value)), myPalette{ text_colour, colors::Black }
		{
			static_assert(std::move_constructible<T>);
		}

		constexpr Dye(T&& value, Colour&& text_colour)
			noexcept(std::is_nothrow_move_constructible_v<T>)
			: myData(std::move(value)), myPalette{ std::move(text_colour), colors::Black }
		{
			static_assert(std::move_constructible<T>);
		}

		constexpr Dye(T&& value, const Palette& palette)
			noexcept(std::is_nothrow_move_constructible_v<T>)
			: myData(std::move(value)), myPalette(palette)
		{
			static_assert(std::move_constructible<T>);
		}

		constexpr Dye(T&& value, Palette&& palette)
			noexcept(std::is_nothrow_move_constructible_v<T>)
			: myData(std::move(value)), myPalette(std::move(palette))
		{
			static_assert(std::move_constructible<T>);
		}

		constexpr void SetData(const T& value) noexcept(std::is_nothrow_copy_assignable_v<T>)
			requires std::copyable<T>
		{
			myData = value;
		}

		constexpr void SetData(T&& value) noexcept(std::is_nothrow_copy_assignable_v<T>)
			requires std::movable<T>
		{
			myData = std::move(value);
		}

		[[nodiscard]]
		constexpr T& GetData() & noexcept
		{
			return myData;
		}

		[[nodiscard]]
		constexpr const T& GetData() const& noexcept
		{
			return myData;
		}

		[[nodiscard]]
		constexpr T&& GetData() && noexcept
		{
			return std::move(myData);
		}

		[[nodiscard]]
		constexpr const T&& GetData() const&& noexcept
		{
			return std::move(myData);
		}

		[[nodiscard]]
		constexpr const Palette& GetPalette() const& noexcept
		{
			return myPalette;
		}

		[[nodiscard]]
		constexpr const Palette&& GetPalette() && noexcept
		{
			return std::move(myPalette);
		}

		[[nodiscard]]
		explicit constexpr operator T() const noexcept(std::is_nothrow_copy_constructible_v<T>)
			requires std::copyable<T>
		{
			return myData;
		}

	private:
		T myData;
		Palette myPalette;
	};
}

export template<typename T>
struct std::formatter<iconer::util::cfc::Dye<T>, char>
{
	static std::format_parse_context::iterator parse(std::format_parse_context& context) noexcept
	{
		return context.begin();
	}

	static auto format(const iconer::util::cfc::Dye<T>& dye, std::format_context& context) noexcept
	{
		return std::format_to(context.out(), "Dye ({})", dye.GetData());
	}
};

export template<typename T>
struct std::formatter<iconer::util::cfc::Dye<T>, wchar_t>
{
	static std::format_parse_context::iterator parse(std::wformat_parse_context& context) noexcept
	{
		return context.begin();
	}

	static auto format(const iconer::util::cfc::Dye<T>& dye, std::wformat_context& context) noexcept
	{
		return std::format_to(context.out(), L"Dye ({})", dye.GetData());
	}
};
