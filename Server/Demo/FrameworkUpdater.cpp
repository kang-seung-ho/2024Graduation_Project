module;
#include <cstdio>

module Demo.Framework;
import <string>;
import <string_view>;

void
demo::Framework::Update()
{
	LockPhase();

	char command[256]{};
	constexpr unsigned cmd_size = sizeof(command);

	while (true)
	{
		auto input = ::scanf_s("%s", command, cmd_size);
		if (EOF != input)
		{
			if (command[0] == 'q')
			{
				CancelWorkers();

				break;
			}
		}
	}

	UnlockPhase();
}
