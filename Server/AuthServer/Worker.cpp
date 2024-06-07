#include "Worker.hpp"
#include "Framework.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSock.h>

import <print>;
//import <mutex>;

void
Worker(std::stop_token&& canceller
	, const size_t index
	, Framework& framework
	, class Pipeline& pipeline)
{
	std::println("Worker {} is generated.", index);

	SOCKADDR_IN address{};
	//std::once_flag workerInitializedFlag{};

	auto first_recv = framework.ReceiveFrom(pipeline, address);
	if (not first_recv)
	{
		std::println("Error when generating worker {}: {}", index, first_recv.error());

		return framework.ArriveWorkersIntialized();
	}

	framework.ArriveWorkersIntialized();

	while (true)
	{
		if (canceller.stop_requested())
		{
			break;
		}

		framework.AwaitWork();

		std::println("Worker {}'s turn.", index);

		::LPOVERLAPPED overlapped{};
		::DWORD bytes{};
		::ULONG_PTR key{};

		const ::BOOL result = ::GetQueuedCompletionStatus(framework.iocpHandle
			, std::addressof(bytes)
			, std::addressof(key)
			, std::addressof(overlapped)
			, INFINITE);

		if (0 == result)
		{
			break;
		}

		const auto recv = framework.ReceiveFrom(pipeline, address);

		if (recv)
		{
			framework.NotifyWork();
		}
		else
		{
			std::println("A receive error occured, error code: {}", recv.error());

			break;
		}
	}
}
