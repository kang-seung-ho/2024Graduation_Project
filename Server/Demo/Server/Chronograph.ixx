module;
#include <compare>
#include <chrono>
export module Iconer.Utility.Chronograph;

export namespace iconer::util
{
	template<typename ChronoClock>
	class BasicChronograph
	{
	public:
		static_assert(std::chrono::is_clock_v<ChronoClock>, "Clock type required");

		using clock_t = ChronoClock;
		using rep = clock_t::rep;
		using period = clock_t::period;
		using duration = clock_t::duration;
		using time_point = clock_t::time_point;
		static constexpr bool is_steady = clock_t::is_steady;

		explicit BasicChronograph() noexcept
			: myTime(clock_t::now())
		{
		}

		[[nodiscard]]
		static constexpr duration Seconds(const rep seconds) noexcept
		{
			return std::chrono::duration_cast<duration>(std::chrono::seconds{ seconds });
		}

		[[nodiscard]]
		static constexpr duration MilliSeconds(const rep seconds) noexcept
		{
			return std::chrono::duration_cast<duration>(std::chrono::milliseconds{ seconds });
		}

		[[nodiscard]]
		static constexpr duration NanoSeconds(const rep seconds) noexcept
		{
			return std::chrono::duration_cast<duration>(std::chrono::nanoseconds{ seconds });
		}

		[[nodiscard]]
		bool HasTimePassed(const duration& dur) const noexcept
		{
			return clock_t::now() <= myTime + dur;
		}

		[[nodiscard]]
		bool HasTimePassed(duration&& dur) const noexcept
		{
			return clock_t::now() <= myTime + std::move(dur);
		}

		template<typename Rep, typename Period>
		[[nodiscard]]
		bool HasTimePassed(const std::chrono::duration<Rep, Period>& dur) const noexcept
		{
			return clock_t::now() <= myTime + std::chrono::duration_cast<duration>(dur);
		}

		template<typename Rep, typename Period>
		[[nodiscard]]
		bool HasTimePassed(std::chrono::duration<Rep, Period>&& dur) const noexcept
		{
			return clock_t::now() <= myTime + std::chrono::duration_cast<duration>(std::move(dur));
		}

		[[nodiscard]]
		constexpr bool HasReachedClock(const time_point& clock) const noexcept
		{
			return myTime <= clock;
		}

		[[nodiscard]]
		constexpr bool HasReachedClock(time_point&& clock) const noexcept
		{
			return std::move(clock).time_since_epoch() <= myTime.time_since_epoch();
		}

		template<typename Clock, typename Duration>
		[[nodiscard]]
		constexpr bool HasReachedClock(const std::chrono::time_point<Clock, Duration>& clock) const noexcept
		{
			return myTime <= clock;
		}

		template<typename Clock, typename Duration>
		[[nodiscard]]
		constexpr bool HasReachedClock(std::chrono::time_point<Clock, Duration>&& clock) const noexcept
		{
			return std::move(clock).time_since_epoch() <= myTime.time_since_epoch();
		}

		constexpr bool operator==(const BasicChronograph&) const noexcept = default;
		constexpr std::strong_ordering operator<=>(const BasicChronograph&) const noexcept = default;

		time_point myTime;

	private:
	};

	using Chronograph = BasicChronograph<std::chrono::high_resolution_clock>;
	using SystemChronograph = BasicChronograph<std::chrono::system_clock>;
	using SteadyChronograph = BasicChronograph<std::chrono::steady_clock>;
	using FileChronograph = BasicChronograph<std::chrono::file_clock>;
	using GpsChronograph = BasicChronograph<std::chrono::gps_clock>;
	using UtcChronograph = BasicChronograph<std::chrono::utc_clock>;
	using TaiChronograph = BasicChronograph<std::chrono::tai_clock>;
}
