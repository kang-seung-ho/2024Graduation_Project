#include "Worker.hpp"
#include "Framework.hpp"

void
Worker(std::stop_token&& cancel_token, class Framework& framework, size_t index)
{
	while (true)
	{
		if (cancel_token.stop_requested())
		{
			break;
		}
	}
}
