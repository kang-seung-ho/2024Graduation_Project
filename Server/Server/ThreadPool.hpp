#pragma once
import Iconer.Net.IoCompletionPort;
import Iconer.Net.ThreadPool;
import <print>;

class ServerThreadPool : public iconer::net::ThreadPool<ServerThreadPool, class ServerFramework, 4>
{
public:
	[[nodiscard]]
	bool OnProcessTask(ServerFramework& framework, iconer::net::IoEvent task);

	void OnInitializeWorker(size_t index)
	{
		std::println("Worker {} is generated.", index);
	}

	void OnTerminateWorker(size_t index)
	{
		std::println("Worker {} is terminated.", index);
	}
};

