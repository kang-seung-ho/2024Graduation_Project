export module Iconer.Utility.Concurrency.SpinLock;
import <atomic>;
import <string>;
import <format>;

export namespace iconer::util
{
	class SpinLock
	{
	public:
		constexpr SpinLock() noexcept
			: mySwitch()
		{
		}

		~SpinLock() noexcept
		{
			mySwitch.clear();
		}

		void lock(const std::memory_order& order = std::memory_order_acquire) noexcept
		{
			while (mySwitch.test_and_set(order));
		}

		void lock(const std::memory_order& order = std::memory_order_acquire) volatile noexcept
		{
			while (mySwitch.test_and_set(order));
		}

		void unlock(const std::memory_order& order = std::memory_order_release) noexcept
		{
			mySwitch.clear(order);
		}

		void unlock(const std::memory_order& order = std::memory_order_release) volatile noexcept
		{
			mySwitch.clear(order);
		}

		bool try_lock(const std::memory_order& order = std::memory_order_relaxed) noexcept
		{
			return !mySwitch.test_and_set(order);
		}

		bool try_lock(const std::memory_order& order = std::memory_order_relaxed) volatile noexcept
		{
			return !mySwitch.test_and_set(order);
		}

		void wait_lock(const std::memory_order& order = std::memory_order_relaxed) noexcept
		{
			return mySwitch.wait(true, order);
		}

		void wait_lock(const std::memory_order& order = std::memory_order_relaxed) volatile noexcept
		{
			return mySwitch.wait(true, order);
		}

		void wait_unlock(const std::memory_order& order = std::memory_order_relaxed) noexcept
		{
			return mySwitch.wait(false, order);
		}

		void wait_unlock(const std::memory_order& order = std::memory_order_relaxed) volatile noexcept
		{
			return mySwitch.wait(false, order);
		}

		void notify() noexcept
		{
			return mySwitch.notify_one();
		}

		void notify() volatile noexcept
		{
			return mySwitch.notify_one();
		}

		void awake() noexcept
		{
			return mySwitch.notify_all();
		}

		void awake() volatile noexcept
		{
			return mySwitch.notify_all();
		}

		bool is_locked() const noexcept
		{
			return mySwitch.test(std::memory_order_relaxed);
		}

		bool is_locked() const volatile noexcept
		{
			return mySwitch.test(std::memory_order_relaxed);
		}

		constexpr bool operator==(const SpinLock&) const noexcept = default;

	private:
		SpinLock(const SpinLock&) = delete;
		SpinLock(SpinLock&&) = delete;
		void operator=(const SpinLock&) = delete;
		void operator=(SpinLock&&) = delete;

		std::atomic_flag mySwitch;
	};

	using ::std::memory_order;
	using ::std::memory_order_relaxed;
	using ::std::memory_order_consume;
	using ::std::memory_order_acquire;
	using ::std::memory_order_release;
	using ::std::memory_order_acq_rel;
	using ::std::memory_order_seq_cst;
}

export template<>
struct std::formatter<iconer::util::SpinLock, char>
{
	static constexpr format_parse_context::iterator
		parse(format_parse_context& context)
	{
		auto it = context.begin();
		const auto end = context.end();
		if (it == end or *it != '{')
		{
			throw std::format_error{ "Invalid format string." };
		}

		++it;
		if (it != end and *it != '}')
		{
			throw std::format_error{ "Missing '}' in format string." };
		}

		return it;
	}

	static format_context::iterator
		format(const iconer::util::SpinLock& lock, format_context& context)
		noexcept
	{
		return format_to(context.out(), "SpinLock(locked={})", lock.is_locked());
	}
};

export template<>
struct std::formatter<iconer::util::SpinLock, wchar_t>
{
	static constexpr wformat_parse_context::iterator
		parse(wformat_parse_context& context)
	{
		auto it = context.begin();
		const auto end = context.end();
		if (it == end or *it != L'{')
		{
			throw std::format_error{ "Invalid format string." };
		}

		++it;
		if (it == end or *it != L'}')
		{
			throw std::format_error{ "Missing '}' in format string." };
		}

		return it;
	}

	static wformat_context::iterator
		format(const iconer::util::SpinLock& lock, wformat_context& context)
		noexcept
	{
		return format_to(context.out(), L"SpinLock(locked={})", lock.is_locked());
	}
};
