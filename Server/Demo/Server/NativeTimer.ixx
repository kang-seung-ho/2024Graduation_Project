module;
#include <Windows.h>

export module Iconer.Application.NativeTimer;
import <cstdint>;
import Iconer.Utility.Constraints;
import Iconer.Utility.Handler;

export namespace iconer::app
{
	class [[nodiscard]] NativeTimer : public iconer::util::Handler<std::uintptr_t>
	{
	public:
		using Super = iconer::util::Handler<std::uintptr_t>;
		using HandleType = Super::HandleType;
		using IdType = std::uint32_t;

		explicit NativeTimer() noexcept = default;

		explicit constexpr NativeTimer(const IdType& id, const HandleType& handle) noexcept
			: Super(handle)
			, myId(id)
			, isStopped(false)
		{}

		NativeTimer(NativeTimer&& other) noexcept
			: Super(std::exchange(other.myHandle, HandleType{}))
			, myId(std::exchange(other.myId, 0))
			, isStopped(other.isStopped)
		{}

		NativeTimer& operator=(NativeTimer&& other) noexcept
		{
			myHandle = std::exchange(other.myHandle, HandleType{});
			myId = std::exchange(other.myId, 0);
			isStopped = other.isStopped;
			return *this;
		}

		~NativeTimer() noexcept;

		bool Stop() noexcept;

		[[nodiscard]]
		constexpr IdType GetID() const noexcept
		{
			return myId;
		}

		[[nodiscard]]
		constexpr bool IsStopped() const noexcept
		{
			return isStopped;
		}

		friend class TimerProcedure;

		friend void __stdcall ExecuteCustomTimer(::HWND, std::uint32_t, ::UINT_PTR, unsigned long);

	private:
		NativeTimer(const NativeTimer&) = delete;
		NativeTimer& operator=(const NativeTimer&) = delete;

		IdType myId;
		bool isStopped;
	};

	using NativeTimerProcedure = void(__stdcall*)(::HWND, std::uint32_t, ::UINT_PTR timer_id, unsigned long sys_time);

	class [[nodiscard]] TimerProcedure
	{
	public:
		constexpr TimerProcedure(NativeTimer& timer) noexcept
			: myTimer(&timer), myPeriod(0)
			, isLooping(false)
		{}
		
		constexpr TimerProcedure(NativeTimer& timer, std::uint32_t milliseconds, bool is_loop) noexcept
			: myTimer(&timer), myPeriod(milliseconds)
			, isLooping(is_loop)
		{}

		virtual ~TimerProcedure() noexcept = default;

		virtual bool operator()()
		{
			return true;
		}

		std::uint32_t myPeriod;

		friend void __stdcall ExecuteCustomTimer(::HWND, std::uint32_t, ::UINT_PTR, unsigned long);

	private:
		NativeTimer* myTimer;
		bool isLooping;
	};

	[[nodiscard]] NativeTimer StartTimer(NativeTimer::IdType id, const NativeTimerProcedure& proc, std::uint32_t milliseconds) noexcept;
	[[nodiscard]] NativeTimer StartTimer(NativeTimer::IdType id, TimerProcedure&& proc) noexcept;
}

namespace iconer::app
{
	void __stdcall ExecuteCustomTimer(::HWND hwnd, std::uint32_t msg, ::UINT_PTR timer_id, unsigned long sys_time);
}
