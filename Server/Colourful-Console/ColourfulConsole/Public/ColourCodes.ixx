export module Iconer.Utility.ColourfulConsole.Colour:Codes;
import <cstdint>;
import <limits>;
import <string_view>;
import <format>;

export namespace iconer::util::cfc
{
	enum class [[nodiscard]] ColourCodes : std::uint16_t
	{
		Black = 0,
		Blue = 1,
		Green = 2,
		Aqua = 3,
		Red = 4,
		Purple = 5,
		Yellow = 6,
		White = 7,
		Grey = 8, Gray = Grey,
		LightBlue = 9,
		LightGreen = 10, Lime = LightGreen,
		LightAqua = 11, Teal = LightAqua,
		LightRed = 12,
		LightPurple = 13, Lavender = LightPurple,
		LightYellow = 14,
		BrightYellow = 15,
		Unknown = static_cast<std::uint16_t>(-256)
	};
}

export template<>
struct std::formatter<iconer::util::cfc::ColourCodes, char>
{
	static std::format_parse_context::iterator parse(std::format_parse_context& context) noexcept
	{
		return context.begin();
	}

	static auto format(const iconer::util::cfc::ColourCodes& col, std::format_context& context) noexcept
	{
		switch (col)
		{
			case iconer::util::cfc::ColourCodes::Black:
			{
				return std::format_to(context.out(), "Black");
			}

			case iconer::util::cfc::ColourCodes::Blue:
			{
				return std::format_to(context.out(), "Blue");
			}

			case iconer::util::cfc::ColourCodes::Green:
			{
				return std::format_to(context.out(), "Green");
			}

			case iconer::util::cfc::ColourCodes::Aqua:
			{
				return std::format_to(context.out(), "Aqua");
			}

			case iconer::util::cfc::ColourCodes::Red:
			{
				return std::format_to(context.out(), "Red");
			}

			case iconer::util::cfc::ColourCodes::Purple:
			{
				return std::format_to(context.out(), "Purple");
			}

			case iconer::util::cfc::ColourCodes::Yellow:
			{
				return std::format_to(context.out(), "Yellow");
			}

			case iconer::util::cfc::ColourCodes::White:
			{
				return std::format_to(context.out(), "White");
			}

			case iconer::util::cfc::ColourCodes::Grey:
			{
				return std::format_to(context.out(), "Grey");
			}

			case iconer::util::cfc::ColourCodes::LightBlue:
			{
				return std::format_to(context.out(), "Light Blue");
			}

			case iconer::util::cfc::ColourCodes::LightGreen:
			{
				return std::format_to(context.out(), "Light Green");
			}

			case iconer::util::cfc::ColourCodes::LightRed:
			{
				return std::format_to(context.out(), "Light Red");
			}

			case iconer::util::cfc::ColourCodes::LightPurple:
			{
				return std::format_to(context.out(), "Light Purple");
			}

			case iconer::util::cfc::ColourCodes::LightYellow:
			{
				return std::format_to(context.out(), "Light Yellow");
			}

			case iconer::util::cfc::ColourCodes::BrightYellow:
			{
				return std::format_to(context.out(), "Bright Yellow");
			}

			default:
			{
				return std::format_to(context.out(), "Unknown");
			}
		}
	}
};

export template<>
struct std::formatter<iconer::util::cfc::ColourCodes, wchar_t>
{
	static std::wformat_parse_context::iterator parse(std::wformat_parse_context& context) noexcept
	{
		return context.begin();
	}

	static auto format(const iconer::util::cfc::ColourCodes& col, std::wformat_context& context) noexcept
	{
		switch (col)
		{
			case iconer::util::cfc::ColourCodes::Black:
			{
				return std::format_to(context.out(), L"Black");
			}

			case iconer::util::cfc::ColourCodes::Blue:
			{
				return std::format_to(context.out(), L"Blue");
			}

			case iconer::util::cfc::ColourCodes::Green:
			{
				return std::format_to(context.out(), L"Green");
			}

			case iconer::util::cfc::ColourCodes::Aqua:
			{
				return std::format_to(context.out(), L"Aqua");
			}

			case iconer::util::cfc::ColourCodes::Red:
			{
				return std::format_to(context.out(), L"Red");
			}

			case iconer::util::cfc::ColourCodes::Purple:
			{
				return std::format_to(context.out(), L"Purple");
			}

			case iconer::util::cfc::ColourCodes::Yellow:
			{
				return std::format_to(context.out(), L"Yellow");
			}

			case iconer::util::cfc::ColourCodes::White:
			{
				return std::format_to(context.out(), L"White");
			}

			case iconer::util::cfc::ColourCodes::Grey:
			{
				return std::format_to(context.out(), L"Grey");
			}

			case iconer::util::cfc::ColourCodes::LightBlue:
			{
				return std::format_to(context.out(), L"Light Blue");
			}

			case iconer::util::cfc::ColourCodes::LightGreen:
			{
				return std::format_to(context.out(), L"Light Green");
			}

			case iconer::util::cfc::ColourCodes::LightRed:
			{
				return std::format_to(context.out(), L"Light Red");
			}

			case iconer::util::cfc::ColourCodes::LightPurple:
			{
				return std::format_to(context.out(), L"Light Purple");
			}

			case iconer::util::cfc::ColourCodes::LightYellow:
			{
				return std::format_to(context.out(), L"Light Yellow");
			}

			case iconer::util::cfc::ColourCodes::BrightYellow:
			{
				return std::format_to(context.out(), L"Bright Yellow");
			}

			default:
			{
				return std::format_to(context.out(), L"Unknown");
			}
		}
	}
};
