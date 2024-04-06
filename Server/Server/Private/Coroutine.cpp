module;
#include <thread>
module Iconer.Coroutine;

void
iconer::coroutine::Coroutine::Start()
{
	if (myHandle and not isTriggered)
	{
		isTriggered = true;

		myHandle();
	}
}

void
iconer::coroutine::Coroutine::StartAsync()
{
	if (myHandle and not isTriggered)
	{
		isTriggered = true;
		static const auto pred = [](std::coroutine_handle<void> handle) -> void {
			if (handle)
			{
				handle();
			}
		};

		std::thread{ pred, myHandle }.detach();
	}
}

void
iconer::coroutine::Coroutine::Resume()
{
	if (myHandle and isTriggered)
	{
		myHandle();
	}
}

void
iconer::coroutine::Coroutine::operator()()
{
	if (myHandle and isTriggered)
	{
		myHandle();
	}
}
