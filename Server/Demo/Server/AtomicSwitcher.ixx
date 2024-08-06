export module Iconer.Utility.AtomicSwitcher;
import Iconer.Utility.Constraints;
import Iconer.Utility.Atomic;
import <type_traits>;

export namespace iconer::util
{
	template<typename Atomic>
	class AtomicSwitcher
	{
	public:
		static_assert(specializations<Atomic, std::atomic>);

		using data_t = Atomic;
		using value_type = Atomic::value_type;

		explicit AtomicSwitcher(data_t& target
			, const std::memory_order init_order = std::memory_order_acquire
			, const std::memory_order final_order = std::memory_order_release)
			noexcept(nothrow_default_constructibles<value_type> and nothrow_atomic_readable<data_t>)
			: myTarget(target), myValue(target.load(init_order))
			, finalOrder(final_order)
		{
		}

		template<typename U>
		AtomicSwitcher(const std::atomic<U>&&, const std::memory_order init_order = std::memory_order_acquire, const std::memory_order final_order = std::memory_order_release) = delete;

		~AtomicSwitcher()
			noexcept(nothrow_move_constructibles<value_type> and nothrow_destructibles<value_type> and nothrow_atomic_writable<data_t>)
		{
			myTarget.store(std::move(myValue), finalOrder);
		}

		template<typename U>
		constexpr void SetValue(U&& value) noexcept(nothrow_assignable<value_type, U&&>)
		{
			myValue = std::forward<U>(value);
		}

		template<typename U>
		constexpr void SetValue(U&& value) volatile noexcept(nothrow_assignable<volatile value_type, U&&>)
		{
			myValue = std::forward<U>(value);
		}

		[[nodiscard]]
		constexpr value_type& GetValue() noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr const value_type& GetValue() const noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr volatile value_type& GetValue() volatile noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr const volatile value_type& GetValue() const volatile noexcept
		{
			return myValue;
		}

		data_t& myTarget;
		value_type myValue;
		std::memory_order finalOrder;
	};

	template<typename T>
	AtomicSwitcher(std::atomic<T>&) -> AtomicSwitcher<std::atomic<T>>;
	template<typename T>
	AtomicSwitcher(volatile std::atomic<T>&) -> AtomicSwitcher<volatile std::atomic<T>>;
	template<typename T>
	AtomicSwitcher(std::atomic<T>&, std::memory_order, std::memory_order) -> AtomicSwitcher<std::atomic<T>>;
	template<typename T>
	AtomicSwitcher(volatile std::atomic<T>&, std::memory_order, std::memory_order) -> AtomicSwitcher<volatile std::atomic<T>>;
}
