#pragma once
import Iconer.Net.IoCompletionPort;
import Iconer.Net.IoResult;
import Iconer.Net.IThreadPool;
import <print>;

class ServerThreadPool : public iconer::net::IThreadPool
{
public:
	using super = iconer::net::IThreadPool;

	ServerThreadPool()
		: super(4)
	{}

	iconer::net::IoResult Initialize() override
	{
		super::Initialize();

		eventOnWorkerInitialized.Add(iconer::util::MakeInvoker(this, &ServerThreadPool::OnStartWorker));
		eventOnWorkerAnnihilated.Add(iconer::util::MakeInvoker(this, &ServerThreadPool::OnTerminateWorker));

		return {};
	}

	void OnStartWorker(size_t index);
	void OnTerminateWorker() const;
};

