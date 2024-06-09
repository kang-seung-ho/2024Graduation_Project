export module Iconer.Utility.ColourfulConsole;
export import Iconer.Utility.ColourfulConsole.Colour;
export import Iconer.Utility.ColourfulConsole.Palette;
export import Iconer.Utility.ColourfulConsole.Dye;
import <utility>;
import <iostream>;

export namespace iconer::util::cfc
{
	void SetConsoleColour(const Palette& pal) noexcept;
	void SetConsoleColour(Palette&& pal) noexcept;
	[[nodiscard]] Palette GetConsoleColour() noexcept;

	void ConsoleTextColour(const Colour& colour) noexcept;
	void ConsoleBackgroundColour(const Colour& colour) noexcept;
	void ConsoleTextColour(Colour&& colour) noexcept;
	void ConsoleBackgroundColour(Colour&& colour) noexcept;
	void ConsoleTextColour(size_t index) noexcept;
	void ConsoleBackgroundColour(size_t index) noexcept;
	[[nodiscard]] Colour ConsoleTextColour() noexcept;
	[[nodiscard]] Colour ConsoleBackgroundColour() noexcept;

	std::ostream& BlackForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::Black);
		return stream;
	}

	std::ostream& BlueForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::Blue);
		return stream;
	}

	std::ostream& GreenForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::Green);
		return stream;
	}

	std::ostream& AquaForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::Aqua);
		return stream;
	}

	std::ostream& RedForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::Red);
		return stream;
	}

	std::ostream& PurpleForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::Purple);
		return stream;
	}

	std::ostream& YellowForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::Yellow);
		return stream;
	}

	std::ostream& WhiteForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::White);
		return stream;
	}

	std::ostream& GreyForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::Grey);
		return stream;
	}

	std::ostream& LtBlueForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::LightBlue);
		return stream;
	}

	std::ostream& LtGreenForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::LightGreen);
		return stream;
	}

	std::ostream& LtAquaForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::LightAqua);
		return stream;
	}

	std::ostream& LtRedForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::LightRed);
		return stream;
	}

	std::ostream& LtPurpleForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::LightPurple);
		return stream;
	}

	std::ostream& LtYellowForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::LightYellow);
		return stream;
	}

	std::ostream& BrightYellowForeground(std::ostream& stream)
	{
		ConsoleTextColour(colors::BrightYellow);
		return stream;
	}

	std::ostream& BlackBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::Black);
		return stream;
	}

	std::ostream& BlueBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::Blue);
		return stream;
	}

	std::ostream& GreenBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::Green);
		return stream;
	}

	std::ostream& AquaBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::Aqua);
		return stream;
	}

	std::ostream& RedBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::Red);
		return stream;
	}

	std::ostream& PurpleBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::Purple);
		return stream;
	}

	std::ostream& YellowBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::Yellow);
		return stream;
	}

	std::ostream& WhiteBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::White);
		return stream;
	}

	std::ostream& GreyBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::Grey);
		return stream;
	}

	std::ostream& LtBlueBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::LightBlue);
		return stream;
	}

	std::ostream& LtGreenBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::LightGreen);
		return stream;
	}

	std::ostream& LtAquaBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::LightAqua);
		return stream;
	}

	std::ostream& LtRedBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::LightRed);
		return stream;
	}

	std::ostream& LtPurpleBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::LightPurple);
		return stream;
	}

	std::ostream& LtYellowBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::LightYellow);
		return stream;
	}

	std::ostream& BrightYellowBackground(std::ostream& stream)
	{
		ConsoleBackgroundColour(colors::BrightYellow);
		return stream;
	}
}

export template<typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>&
operator<<(std::basic_ostream<Elem, Traits>& stream, const iconer::util::cfc::Colour& colour)
noexcept
{
	iconer::util::cfc::ConsoleTextColour(colour);

	return stream;
}

export template<typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>&
operator<<(std::basic_ostream<Elem, Traits>& stream, iconer::util::cfc::Colour&& colour)
noexcept
{
	iconer::util::cfc::ConsoleTextColour(std::move(colour));

	return stream;
}

export template<typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>&
operator<<(std::basic_ostream<Elem, Traits>& stream, const iconer::util::cfc::Palette& pal)
noexcept
{
	iconer::util::cfc::SetConsoleColour(pal);

	return stream;
}

export template<typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>&
operator<<(std::basic_ostream<Elem, Traits>& stream, iconer::util::cfc::Palette&& pal)
noexcept
{
	iconer::util::cfc::SetConsoleColour(std::move(pal));

	return stream;
}

export template<typename Elem, typename Traits, typename T>
std::basic_ostream<Elem, Traits>&
operator<<(std::basic_ostream<Elem, Traits>& stream, iconer::util::cfc::Dye<T>& dye)
noexcept(noexcept(std::declval<std::basic_ostream<Elem, Traits>&>()))
{
	iconer::util::cfc::SetConsoleColour(dye.GetPalette());
	stream << dye.GetData();

	return stream;
}

export template<typename Elem, typename Traits, typename T>
std::basic_ostream<Elem, Traits>&
operator<<(std::basic_ostream<Elem, Traits>& stream, const iconer::util::cfc::Dye<T>& dye)
{
	iconer::util::cfc::SetConsoleColour(dye.GetPalette());
	stream << dye.GetData();

	return stream;
}

export template<typename Elem, typename Traits, typename T>
std::basic_ostream<Elem, Traits>&
operator<<(std::basic_ostream<Elem, Traits>& stream, iconer::util::cfc::Dye<T>&& dye)
{
	iconer::util::cfc::SetConsoleColour(std::move(dye).GetPalette());
	stream << std::move(dye).GetData();

	return stream;
}

export template<typename Elem, typename Traits, typename T>
std::basic_ostream<Elem, Traits>&
operator<<(std::basic_ostream<Elem, Traits>& stream, const iconer::util::cfc::Dye<T>&& dye)
{
	iconer::util::cfc::SetConsoleColour(std::move(dye).GetPalette());
	stream << std::move(dye).GetData();

	return stream;
}
