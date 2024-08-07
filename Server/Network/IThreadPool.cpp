module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Net.IThreadPool;
import Iconer.Net.Socket;
import Iconer.Net.IoContext;
import <type_traits>;
import <print>;

iconer::net::IoResult
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

	return {};
}

void
iconer::net::IThreadPool::Startup()
{
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
}

void
iconer::net::IThreadPool::Cleanup()
{
	StopWorkers();

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

			//break;
		}
#else
			if (not self.ProcessTask(self.AwaitForTask())) UNLIKELY
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
