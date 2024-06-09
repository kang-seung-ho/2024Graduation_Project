module;
module Demo.Framework;
import <string>;
import <string_view>;

void
demo::Framework::Start()
{
	LockPhase();
	myLogger.Log(L"\topening the listener...\n");

	if (not serverListener.Open().has_value())
	{
		myLogger.Log(L"\tThe listener is opened.\n");
	}

	myLogger.Log(L"\treserving acceptance of users...\n");

	if (not StartAccepts())
	{
		throw std::exception{ "Error when reserving acceptance of a socket." };
	}

	UnlockPhase();
}
