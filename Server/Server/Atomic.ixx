export module Iconer.Utility.Atomic;
import Iconer.Utility.Constraints;
export import <atomic>;

export namespace iconer::util
{
	template <typename T>
	struct is_nothrow_atomic_readable : false_type
	{};

	template<typename T>
	struct is_nothrow_atomic_readable<std::atomic<T>>
		: std::bool_constant<noexcept(std::declval<std::atomic<T>>().load(std::declval<std::memory_order>()))>
	{};

	template<typename T>
	inline constexpr bool is_nothrow_atomic_readable_v = is_nothrow_atomic_readable<T>::template value;

	template<typename T>
	concept nothrow_atomic_readable = is_nothrow_atomic_readable_v<T>;

	template <typename T>
	struct is_nothrow_atomic_writable : false_type
	{};

	template<typename T>
	struct is_nothrow_atomic_writable<std::atomic<T>>
		: std::bool_constant<noexcept(std::declval<std::atomic<T>>().store(std::declval<T>(), std::declval<std::memory_order>()))>
	{};

	template <typename T, typename U>
	struct is_nothrow_atomic_writable_with : false_type
	{};

	template<typename T, typename U>
	struct is_nothrow_atomic_writable_with<std::atomic<T>, U>
		: std::bool_constant<noexcept(std::declval<std::atomic<T>>().store(std::declval<U>(), std::declval<std::memory_order>()))>
	{};

	template<typename T>
	inline constexpr bool is_nothrow_atomic_writable_v = is_nothrow_atomic_writable<T>::template value;
	
	template<typename T, typename U>
	inline constexpr bool is_nothrow_atomic_writable_with_v = is_nothrow_atomic_writable_with<T, U>::template value;

	template<typename T, typename U>
	concept nothrow_atomic_writable_with = specializations<T, std::atomic> and is_nothrow_atomic_writable_with_v<T, U>;

	template<typename T>
	concept nothrow_atomic_writable = specializations<T, std::atomic> and is_nothrow_atomic_writable_v<T>;
}
