#include "Framework.hpp"

Framework serverInstance{};

int main()
{
	serverInstance.Initialize();
	serverInstance.Startup();

	return 0;
}

void
ExitHandler()
noexcept
{
	serverInstance.Cleanup();
}
