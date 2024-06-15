#pragma once
import Iconer.Net.IoCompletionPort;
import Iconer.Net.IThreadPool;
import <print>;

class ServerThreadPool : public iconer::net::IThreadPool<ServerThreadPool, class ServerFramework, 4>
{
public:
	static inline constexpr size_t workerCount = 4;

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

