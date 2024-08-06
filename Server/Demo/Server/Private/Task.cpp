module;
#include <exception>
#include <thread>
module Iconer.Coroutine.Task;

iconer::coroutine::Task<void>
iconer::coroutine::Task<void>::promise_type::get_return_object() noexcept
{
	return Task<void>(handle_type::from_promise(*this));
}

iconer::coroutine::Task<void>::~Task()
noexcept
{
	if (myHandle)
	{
		myHandle.destroy();
	}
}

void
iconer::coroutine::Task<void>::Start()
const
{
	iconer::coroutine::__ExecuteCoroutine(myHandle);
}

void
iconer::coroutine::Task<void>::StartAsync()
const
{
	iconer::coroutine::__ExecuteCoroutineAsync(myHandle);
}

void
iconer::coroutine::Task<void>::operator()()
const
{
	iconer::coroutine::__ExecuteCoroutine(myHandle);
}

iconer::coroutine::Task<void>::Awaiter
iconer::coroutine::Task<void>::operator co_await()
const noexcept
{
	return Awaiter{ .coHandle = myHandle };
}

bool
iconer::coroutine::Task<void>::IsDone()
const noexcept
{
	return myHandle.done();
}

bool
iconer::coroutine::Task<void>::operator==(const iconer::coroutine::Task<void>& other)
const noexcept
{
	return myHandle.address() == other.myHandle.address();
}

void
iconer::coroutine::__ExecuteCoroutine(std::coroutine_handle<void> handle)
{
	if (handle)
	{
		handle.resume();
	}
}

void iconer::coroutine::__ExecuteCoroutineAsync(std::coroutine_handle<void> handle)
{
	if (handle)
	{
		std::thread{
			[h = std::move(handle)] {
			h.resume();
		} }.detach();
	}
}
