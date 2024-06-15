#include "ThreadPool.hpp"
#include "Framework.hpp"

import Iconer.App.TaskContext;

thread_local constinit size_t workerId{};

void
ServerThreadPool::OnStartWorker(size_t index)
{
	std::println("Worker {} is generated.", index);

	workerId = index;
}

void
ServerThreadPool::OnTerminateWorker() const
{
	std::println("Worker {} is terminated.", workerId);
}

void
ServerThreadPool::OnTaskSucceed(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes) const
{
	const auto task_ctx = static_cast<iconer::app::TaskContext*>(context);

	std::println("[Task({})] id={} ({} bytes)", static_cast<void*>(context), id, bytes);
	if (nullptr == task_ctx)
	{
		return;
	}

	using namespace iconer::net;
	using enum iconer::app::TaskCategory;

	switch (task_ctx->myCategory)
	{
	default:
	{}
	break;
	}
}

void
ServerThreadPool::OnTaskFailure(iconer::net::IoContext * context, std::uint64_t id, std::uint32_t bytes) const
{
	std::println("[Task({})] id={} ({} bytes)", static_cast<void*>(context), id, bytes);
}
