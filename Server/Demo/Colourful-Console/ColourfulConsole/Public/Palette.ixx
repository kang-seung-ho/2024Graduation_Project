export module Iconer.Utility.ColourfulConsole.Palette;
export import Iconer.Utility.ColourfulConsole.Colour;

export namespace iconer::util::cfc
{
	struct [[nodiscard]] Palette
	{
		Colour fgColour;
		Colour bgColour;
	};
}
