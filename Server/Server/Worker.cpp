#include "Worker.hpp"
#include "Framework.hpp"

import <type_traits>;
import <print>;

void
Worker(std::stop_token&& cancel_token, class Framework& framework, size_t index)
{
	std::println("Worker {} is generated.", index);

	framework.ArriveWorker();

	while (true)
	{
		if (cancel_token.stop_requested()) UNLIKELY
		{
			break;
		}

#if _DEBUG
		auto task = framework.AwaitForTask();
		std::println("[Task({})] id={} ({} bytes)"
			, task.isSucceed, task.eventId, task.ioBytes);

		if (not framework.ProcessTask(std::move(task))) UNLIKELY
		{
			if (not cancel_token.stop_requested()) UNLIKELY
			{
				std::println("An issue appeared at worker {}.", index);
			}

			break;
		}
#else
		if (not framework.ProcessTask(framework.AwaitForTask())) UNLIKELY
		{
			break;
		}
#endif
	}

	std::println("Worker {} is terminated.", index);
}
