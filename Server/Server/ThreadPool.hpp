#pragma once
import Iconer.Net.IoCompletionPort;
import Iconer.Net.IThreadPool;
import <print>;

class ServerThreadPool : public iconer::net::IThreadPool
{
public:
	using super = iconer::net::IThreadPool;

	ServerThreadPool()
		: super(4)
	{}

	std::expected<void, iconer::net::ErrorCode> Initialize() override
	{
		super::Initialize();

		eventOnWorkerInitialized.Add(iconer::util::MakeInvoker(this, &ServerThreadPool::OnStartWorker));
		eventOnWorkerAnnihilated.Add(iconer::util::MakeInvoker(this, &ServerThreadPool::OnTerminateWorker));

		eventOnTaskFailure.Add(iconer::util::MakeInvoker(this, &ServerThreadPool::OnTaskFailed));

		return {};
	}

	void OnStartWorker(size_t index)
	{
		std::println("Worker {} is generated.", index);
	}

	void OnTerminateWorker() const
	{
		std::println("Worker is terminated.");
	}

	void OnTaskFailed(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes) const
	{
		std::println("[Task({})] id={} ({} bytes)", static_cast<void*>(context), id, bytes);
	}
};

