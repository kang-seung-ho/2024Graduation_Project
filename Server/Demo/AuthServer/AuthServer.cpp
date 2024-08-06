#include "Framework.hpp"

auth::Server framework{};

int main()
{
	if (auto init = framework.Initialize(); not init)
	{
		return init.error();
	}
	
	if (auto start = framework.Startup(); not start)
	{
		return start.error();
	}

	framework.Cleanup();

	return 0;
}

void
auth::ExitHandler()
noexcept
{
	framework.Cleanup();
}
