#pragma comment(lib, "ColourfulConsole.lib")
import Iconer.Utility.ColourfulConsole;
import <iostream>;

int main()
{
	std::cout << "Hello World!\n";

	using namespace iconer::util::cfc;

	const auto fgcol = ConsoleTextColour();
	const auto bgcol = ConsoleBackgroundColour();
	const auto pal = GetConsoleColour();

	std::cout << "fgcol: " << fgcol.GetIntegerCode() << '\n';
	std::cout << "bgcol: " << bgcol.GetIntegerCode() << '\n';
	std::cout << '\n';

	//ConsoleTextColour(colors::Black);
	std::cout << BlackForeground << "fgcol (Black)";
	//ConsoleTextColour(colors::White);
	std::cout << WhiteForeground << "<--- black is here\n";
	//ConsoleTextColour(colors::Blue);
	std::cout << BlueForeground << "fgcol (Blue)\n";
	//ConsoleTextColour(colors::Green);
	std::cout << GreenForeground << "fgcol (Green)\n";
	//ConsoleTextColour(colors::Aqua);
	std::cout << AquaForeground << "fgcol (Aqua)\n";
	//ConsoleTextColour(colors::Red);
	std::cout << RedForeground << "fgcol (Red)\n";
	//ConsoleTextColour(colors::Purple);
	std::cout << PurpleForeground << "fgcol (Purple)\n";
	//ConsoleTextColour(colors::Yellow);
	std::cout << YellowForeground << "fgcol (Yellow)\n";
	//ConsoleTextColour(colors::White);
	std::cout << WhiteForeground << "fgcol (White)\n";
	//ConsoleTextColour(colors::Grey);
	std::cout << GreyForeground << "fgcol (Grey)\n";
	//ConsoleTextColour(colors::LightBlue);
	std::cout << LtBlueForeground << "fgcol (LightBlue)\n";
	//ConsoleTextColour(colors::LightGreen);
	std::cout << LtGreenForeground << "fgcol (LightGreen)\n";
	//ConsoleTextColour(colors::LightAqua);
	std::cout << LtAquaForeground << "fgcol (LightAqua)\n";
	//ConsoleTextColour(colors::LightRed);
	std::cout << LtRedForeground << "fgcol (LightRed)\n";
	//ConsoleTextColour(colors::LightPurple);
	std::cout << LtPurpleForeground << "fgcol (LightPurple)\n";
	//ConsoleTextColour(colors::LightYellow);
	std::cout << LtYellowForeground << "fgcol (LightYellow)\n";
	//ConsoleTextColour(colors::BrightYellow);
	std::cout << BrightYellowForeground << "fgcol (BrightYellow)\n";
	std::cout << '\n';

	//ConsoleBackgroundColour(colors::BrightYellow);
	std::cout << BrightYellowBackground << "bgcol (BrightYellow)";
	ConsoleTextColour(colors::White);
	ConsoleBackgroundColour(colors::Black);
	std::cout << "<--- bright yellow is here\n";
	ConsoleTextColour(colors::BrightYellow);
	//ConsoleBackgroundColour(colors::LightYellow);
	std::cout << LtYellowBackground << "bgcol (LightYellow)\n";
	//ConsoleBackgroundColour(colors::LightPurple);
	std::cout << LtPurpleBackground << "bgcol (LightPurple)\n";
	//ConsoleBackgroundColour(colors::LightRed);
	std::cout << LtRedBackground << "bgcol (LightRed)\n";
	//ConsoleBackgroundColour(colors::LightAqua);
	std::cout << LtAquaBackground << "bgcol (LightAqua)\n";
	//ConsoleBackgroundColour(colors::LightGreen);
	std::cout << LtGreenBackground << "bgcol (LightGreen)\n";
	//ConsoleBackgroundColour(colors::LightBlue);
	std::cout << LtBlueBackground << "bgcol (LightBlue)\n";
	//ConsoleBackgroundColour(colors::Grey);
	std::cout << GreyBackground << "bgcol (Grey)\n";
	//ConsoleBackgroundColour(colors::White);
	std::cout << WhiteBackground << "bgcol (White)\n";
	//ConsoleBackgroundColour(colors::Yellow);
	std::cout << YellowBackground << "bgcol (Yellow)\n";
	//ConsoleBackgroundColour(colors::Purple);
	std::cout << PurpleBackground << "bgcol (Purple)\n";
	//ConsoleBackgroundColour(colors::Red);
	std::cout << RedBackground << "bgcol (Red)\n";
	//ConsoleBackgroundColour(colors::Aqua);
	std::cout << AquaBackground << "bgcol (Aqua)\n";
	//ConsoleBackgroundColour(colors::Green);
	std::cout << GreenBackground << "bgcol (Green)\n";
	//ConsoleBackgroundColour(colors::Blue);
	std::cout << BlueBackground << "bgcol (Blue)\n";
	//ConsoleBackgroundColour(colors::Black);
	std::cout << BlackBackground << "bgcol (Black)\n";
	std::cout << '\n';

	for (size_t i = 15; 0 < i; --i)
	{
		ConsoleTextColour(i);
		std::cout << '@';
	}
	std::cout << '\n';

	ConsoleTextColour(colors::Black);
	for (size_t i = 0; i < 16; ++i)
	{
		ConsoleBackgroundColour(i);
		std::cout << '@';
	}
	std::cout << "\n\n";

	for (size_t i = 0; i < 16; ++i)
	{
		ConsoleTextColour(i);

		for (size_t j = 15; 0 < j; --j)
		{
			ConsoleBackgroundColour(j);
			std::cout << '@';
		}

		ConsoleBackgroundColour(colors::Black);
		std::cout << '\n';
	}

	ConsoleTextColour(colors::White);
	std::cout << "\n\n";
}
