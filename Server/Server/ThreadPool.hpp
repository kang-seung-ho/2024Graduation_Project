#pragma once
import Iconer.Net.IoCompletionPort;
import Iconer.Net.IThreadPool;
import <print>;

class ServerThreadPool : public iconer::net::IThreadPool<ServerThreadPool, class ServerFramework, 4>
{
public:
	using super = iconer::net::IThreadPool<ServerThreadPool, class ServerFramework, 4>;

	size_t myIndex;

	ServerThreadPool()
		: super()
		, myIndex()
	{}

	std::expected<void, iconer::net::ErrorCode> Initialize()
	{
		eventOnStarted.Add(iconer::util::MakeInvoker(this, &ServerThreadPool::OnStartWorker));
		eventOnDestructed.Add(iconer::util::MakeInvoker(this, &ServerThreadPool::OnTerminateWorker));

		return super::Initialize();
	}

	void OnStartWorker(size_t index)
	{
		std::println("Worker {} is generated.", index);

		myIndex = index;
	}

	void OnTerminateWorker() const
	{
		std::println("Worker {} is terminated.", myIndex);
	}
};

