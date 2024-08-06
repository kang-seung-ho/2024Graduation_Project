#include "Worker.hpp"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>

#include "Framework.hpp"
#include "IoContext.hpp"

import <new>;
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

		const auto ctx = static_cast<auth::IoContext*>(overlapped);
		if (nullptr == ctx)
		{
			std::println("A task error occured at worker {}. The context is not `IoContext`.", index);

			goto finished;
		}

		switch (ctx->myCategory)
		{
		case auth::IoCategory::None:
		{
			std::println("A task error occured at worker {}. The context has `None` task category.", index);

			delete ctx;
		}
		break;

		case auth::IoCategory::Recv:
		{
			instance.OnReceived(bytes);
			instance.EndReceive();

			// restart
			if (const auto recv = instance.BeginReceive(); not recv)
			{
				std::println("A receiving error occured, error code: {}", recv.error());

				goto finished;
			}
		}
		break;

		case auth::IoCategory::Send:
		{
			const auto context = static_cast<auth::SendContext*>(ctx);

			instance.OnSent(context);
			instance.EndSend(context);
		}
		break;

		case auth::IoCategory::CheckUser:
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
