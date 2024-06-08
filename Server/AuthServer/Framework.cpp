#include "Framework.hpp"
#pragma warning(push)
#pragma warning(disable : 4006)
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#pragma warning(pop)

void
Framework::ArriveWorkersIntialized()
noexcept
{
	return workerInitializationSynchronizer.arrive_and_wait();
}

void
Framework::AwaitWork()
noexcept
{
	iocpBarrier.wait(false);

	iocpBarrier = false;
}

void
Framework::NotifyWork()
noexcept
{
	iocpBarrier = true;
	return iocpBarrier.notify_one();
}
