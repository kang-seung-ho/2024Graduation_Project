module;
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

module Iconer.Utility.ColourfulConsole;
import <cstdint>;
import <utility>;
import <atomic>;

namespace
{
	inline constexpr std::uint16_t bad_colour_id = static_cast<std::uint16_t>(iconer::util::cfc::ColourCodes::Unknown);
	constinit ::HANDLE console_handle{};
	std::atomic_bool handle_ready{};

	inline void AcquireConsoleHandle() noexcept
	{
		bool expected = false;

		if (handle_ready.compare_exchange_strong(expected, true))
		{
			console_handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
		}
	}

	inline std::uint16_t AcquireConsoleColours() noexcept
	{
		AcquireConsoleHandle();

		::CONSOLE_SCREEN_BUFFER_INFO console_info{};

		const BOOL succeed = ::GetConsoleScreenBufferInfo(console_handle, std::addressof(console_info));

		if (TRUE == succeed)
		{
			return console_info.wAttributes;
		}
		else
		{
			return bad_colour_id;
		}
	}

	[[nodiscard]]
	constexpr
		std::uint16_t
		Mix(const iconer::util::cfc::Colour& lhs, const iconer::util::cfc::Colour& rhs)
		noexcept
	{
		std::uint16_t mixed_colour = static_cast<std::uint16_t>(lhs.myCode) + static_cast<std::uint16_t>(rhs.myCode) * 16U;
		if (mixed_colour < 256)
		{
			return mixed_colour;
		}
		else
		{
			return bad_colour_id;
		}
	}

	[[nodiscard]]
	constexpr
		std::uint16_t
		Invert(const std::uint16_t& mixed_colour)
		noexcept
	{
		if (mixed_colour < 256)
		{
			std::uint16_t a = mixed_colour % 16U;
			std::uint16_t b = mixed_colour / 16U;

			return b + a * 16U;
		}
		else
		{
			return bad_colour_id;
		}
	}

	inline constexpr iconer::util::cfc::Colour indexed_colors[] =
	{
		iconer::util::cfc::colors::Black,
		iconer::util::cfc::colors::Blue,
		iconer::util::cfc::colors::Green,
		iconer::util::cfc::colors::Aqua,
		iconer::util::cfc::colors::Red,
		iconer::util::cfc::colors::Purple,
		iconer::util::cfc::colors::Yellow,
		iconer::util::cfc::colors::White,
		iconer::util::cfc::colors::Grey,
		iconer::util::cfc::colors::LightBlue,
		iconer::util::cfc::colors::LightGreen,
		iconer::util::cfc::colors::LightAqua,
		iconer::util::cfc::colors::LightRed,
		iconer::util::cfc::colors::LightPurple,
		iconer::util::cfc::colors::LightYellow,
		iconer::util::cfc::colors::BrightYellow,
	};
}

void
iconer::util::cfc::SetConsoleColour(const iconer::util::cfc::Palette& pal)
noexcept
{
	SetConsoleTextAttribute(console_handle, Mix(pal.fgColour, pal.bgColour));
}

void
iconer::util::cfc::SetConsoleColour(iconer::util::cfc::Palette&& pal)
noexcept
{
	SetConsoleTextAttribute(console_handle, Mix(pal.fgColour, pal.bgColour));
}

iconer::util::cfc::Palette
iconer::util::cfc::GetConsoleColour()
noexcept
{
	return Palette{ ConsoleTextColour(), ConsoleBackgroundColour() };
}

void
iconer::util::cfc::ConsoleTextColour(const iconer::util::cfc::Colour& color)
noexcept
{
	SetConsoleColour(Palette{ color, ConsoleBackgroundColour() });
}

void
iconer::util::cfc::ConsoleBackgroundColour(const iconer::util::cfc::Colour& color)
noexcept
{
	SetConsoleColour(Palette{ ConsoleTextColour(), color });
}

void
iconer::util::cfc::ConsoleTextColour(iconer::util::cfc::Colour&& color)
noexcept
{
	SetConsoleColour(Palette{ std::move(color), ConsoleBackgroundColour() });
}

void
iconer::util::cfc::ConsoleBackgroundColour(iconer::util::cfc::Colour&& color)
noexcept
{
	SetConsoleColour(Palette{ ConsoleTextColour(), std::move(color) });
}

void
iconer::util::cfc::ConsoleTextColour(size_t index)
noexcept
{
	ConsoleTextColour(indexed_colors[index]);
}

void
iconer::util::cfc::ConsoleBackgroundColour(size_t index)
noexcept
{
	ConsoleBackgroundColour(indexed_colors[index]);
}

iconer::util::cfc::Colour
iconer::util::cfc::ConsoleTextColour()
noexcept
{
	const auto mixed_colour = AcquireConsoleColours();

	return Colour
	{
		.myCode = static_cast<ColourCodes>((mixed_colour == bad_colour_id) ? bad_colour_id : mixed_colour % 16U)
	};
}

iconer::util::cfc::Colour
iconer::util::cfc::ConsoleBackgroundColour()
noexcept
{
	const auto mixed_colour = AcquireConsoleColours();

	return Colour
	{
		.myCode = static_cast<ColourCodes>((mixed_colour == bad_colour_id) ? bad_colour_id : mixed_colour / 16U)
	};
}
