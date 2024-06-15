module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Net.IThreadPool;
import <type_traits>;
import <print>;

std::expected<void, iconer::net::ErrorCode>
iconer::net::IThreadPool::Initialize()
{
	if (auto io = IoCompletionPort::Create(); io) LIKELY
	{
		ioCompletionPort = std::move(io.value());
	}
	else UNLIKELY
	{
		return std::unexpected{ io.error() };
	};

	if (eventOnInitialied.IsBound())
	{
		eventOnInitialied.Broadcast();
	}

	if (eventOnPostInitialied.IsBound())
	{
		eventOnPostInitialied.Broadcast();
	}

	return {};
}

void
iconer::net::IThreadPool::Startup()
{
	if (eventOnPreStarted.IsBound())
	{
		eventOnPreStarted.Broadcast();
	}

	size_t index{};

	const auto cap = GetMaxWorkerNumber();
	for (size_t i = 0; i < cap; ++i)
	{
		myWorkers.push_back(std::jthread
			{
				&this_class::Worker,
				std::ref(*this),
				++index
			}
		);
	}

	if (eventOnStarted.IsBound())
	{
		eventOnStarted.Broadcast();
	}
}

void
iconer::net::IThreadPool::Cleanup()
{
	if (eventOnDestructed.IsBound())
	{
		eventOnDestructed.Broadcast();
	}

	for (auto& th : myWorkers)
	{
		th.request_stop();
	}

	ioCompletionPort.Destroy();
}

void
iconer::net::IThreadPool::Worker(std::stop_token&& cancel_token, IThreadPool& self, size_t index)
{
	self.ArriveWorker();

	if (self.eventOnWorkerInitialized.IsBound())
	{
		self.eventOnWorkerInitialized.Broadcast(index);
	}

	while (true)
	{
		if (cancel_token.stop_requested()) UNLIKELY
		{
			break;
		}

#if _DEBUG
		iconer::net::IoEvent task = self.AwaitForTask();

		if (not self.ProcessTask(std::move(task))) UNLIKELY
		{
			if (not cancel_token.stop_requested()) UNLIKELY
			{
				std::println("An issue appeared at worker {}.", index);
			}

			break;
		}
#else
			if (not ProcessTask(AwaitForTask())) UNLIKELY
			{
				break;
			}
#endif
	}

	if (self.eventOnWorkerAnnihilated.IsBound())
	{
		self.eventOnWorkerAnnihilated.Broadcast(index);
	}
}

bool
iconer::net::IThreadPool::ProcessTask(iconer::net::IoEvent task)
{
	if (task.isSucceed) LIKELY
	{
		if (eventOnTaskSucceed.IsBound())
		{
			eventOnTaskSucceed.Broadcast(task.ioContext, task.eventId, task.ioBytes);
		}
	}
	else UNLIKELY
	{
		if (eventOnTaskFailure.IsBound())
		{
			eventOnTaskFailure.Broadcast(task.ioContext, task.eventId, task.ioBytes);
		}
	}

	return task.isSucceed;
}
