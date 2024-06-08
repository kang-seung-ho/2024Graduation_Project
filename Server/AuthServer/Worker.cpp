#include "Worker.hpp"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>

#include "Framework.hpp"
#include "IoContext.hpp"

import <print>;
//import <mutex>;

void
Worker(std::stop_token&& canceller
	, const size_t index
	, class auth::Server& instance)
{
	std::println("Worker {} is generated.", index);

	instance.ArriveWorkersIntialized();

	while (true)
	{
		if (canceller.stop_requested()) UNLIKELY
		{
			break;
		}

		std::println("Worker {}'s turn.", index);

		::LPOVERLAPPED overlapped{};
		::DWORD bytes{};
		::ULONG_PTR key{};

		const ::BOOL result = ::GetQueuedCompletionStatus(instance.iocpHandle
			, std::addressof(bytes)
			, std::addressof(key)
			, std::addressof(overlapped)
			, INFINITE);

		if (0 == result)
		{
			break;
		}

		const auto ctx = static_cast<IoContext*>(overlapped);
		if (nullptr == ctx)
		{
			std::println("A task error occured at worker {}. The context is not `IoContext`.", index);

			goto finished;
		}

		switch (ctx->myCategory)
		{
		case IoCategory::None:
		{
			std::println("A task error occured at worker {}. The context has `None` task category.", index);

			delete ctx;
		}
		break;

		case IoCategory::Recv:
		{
			delete ctx;
		}
		break;

		case IoCategory::Send:
		{
			delete ctx;
		}
		break;

		case IoCategory::CheckUser:
		{
			delete ctx;
		}
		break;

		default:
		{
			std::println("A task error occured at worker {}. The context has a invalid task category.", index);

			delete ctx;

			goto finished;
		}
		}
	}

finished:
	std::println("Worker {} is terminated.", index);
}
