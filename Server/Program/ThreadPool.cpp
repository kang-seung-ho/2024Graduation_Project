module;
module Iconer.ThreadPool;
import Iconer.Framework;
import <print>;

thread_local constinit size_t workerId{};

void
ServerThreadPool::OnStartWorker(size_t index)
{
	PrintLn("Worker {} is generated.", index);

	workerId = index;
}

void
ServerThreadPool::OnTerminateWorker() const
{
	PrintLn("Worker {} is terminated.", workerId);
}
