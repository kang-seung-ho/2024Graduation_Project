module;
#include <mutex>
#include <shared_mutex>
export module Iconer.Utility.Concurrency.Lock;

export namespace iconer
{
	template <typename T>
	concept lockable_object = requires(T& lock_obj)
	{
		lock_obj.lock();
		lock_obj.unlock();
	};

	template <typename T>
	concept nothrow_lockable = lockable_object<T> and requires(T& lock_obj)
	{
		{ lock_obj.lock() } noexcept;
	};

	template <typename T>
	concept nothrow_unlockable = lockable_object<T> and requires(T& lock_obj)
	{
		{ lock_obj.unlock() } noexcept;
	};

	template <typename... Ts>
	concept lockable_objects = std::conjunction_v<std::bool_constant<lockable_object<Ts>>...>;

	template <typename... Ts>
	concept nothrow_lockables = lockable_objects<Ts...> and std::conjunction_v<std::bool_constant<nothrow_lockable<Ts>>...>;

	template <typename... Ts>
	concept nothrow_unlockables = lockable_objects<Ts...> and std::conjunction_v<std::bool_constant<nothrow_unlockable<Ts>>...>;
}

export namespace iconer::util
{
	template <lockable_object... Mtxes>
	class [[nodiscard]] ScopedLock
	{
	public:
		explicit ScopedLock(Mtxes&... mtxes) noexcept(nothrow_lockables<Mtxes...>)
			: lockImplementations(std::tie(mtxes...))
		{
			std::lock(mtxes...);
		}

		explicit constexpr ScopedLock(std::adopt_lock_t, Mtxes&... mtxes) noexcept
			: lockImplementations(std::tie(mtxes...))
		{
		}
		
		explicit constexpr ScopedLock(std::defer_lock_t, Mtxes&... mtxes) noexcept
			: lockImplementations(std::tie(mtxes...))
		{
		}

		explicit ScopedLock(std::try_to_lock_t, Mtxes&... mtxes) noexcept(nothrow_lockables<Mtxes...>)
			: lockImplementations(std::tie(mtxes...))
		{
			std::try_lock(mtxes...);
		}

		~ScopedLock() noexcept(nothrow_unlockables<Mtxes...>)
		{
			std::apply([](auto&... lock_obj) { (..., (void)lock_obj.unlock()); }, lockImplementations);
		}

	private:
		ScopedLock(const ScopedLock&) = delete;
		ScopedLock& operator=(const ScopedLock&) = delete;

		std::tuple<Mtxes&...> lockImplementations;
	};

	template <lockable_object Mtx>
	class [[nodiscard]] ScopedLock<Mtx>
	{
	public:
		using mutex_type = Mtx;

		explicit ScopedLock(Mtx& _Mtx) noexcept(nothrow_lockable<Mtx>)
			: lockImplementation(_Mtx)
		{
			lockImplementation.lock();
		}

		explicit constexpr ScopedLock(std::adopt_lock_t, Mtx& _Mtx) noexcept
			: lockImplementation(_Mtx)
		{
		}
		
		explicit constexpr ScopedLock(std::defer_lock_t, Mtx& _Mtx) noexcept
			: lockImplementation(_Mtx)
		{
		}

		explicit ScopedLock(std::try_to_lock_t, Mtx& _Mtx) noexcept(nothrow_lockable<Mtx>)
			: lockImplementation(_Mtx)
		{
			lockImplementation.try_lock();
		}

		~ScopedLock() noexcept(nothrow_unlockable<Mtx>)
		{
			lockImplementation.unlock();
		}

	private:
		ScopedLock(const ScopedLock&) = delete;
		ScopedLock& operator=(const ScopedLock&) = delete;

		Mtx& lockImplementation;
	};

	template <>
	class ScopedLock<>
	{
	public:
		explicit ScopedLock() = default;
		explicit ScopedLock(std::adopt_lock_t) noexcept {}

	private:
		ScopedLock(const ScopedLock&) = delete;
		ScopedLock& operator=(const ScopedLock&) = delete;
	};

	template<lockable_object Mtx, lockable_object... Mtxes>
	ScopedLock(Mtx&, Mtxes&...) -> ScopedLock<Mtx, Mtxes...>;

	template<lockable_object Mtx, lockable_object... Mtxes>
	ScopedLock(volatile Mtx&, Mtxes&...) -> ScopedLock<volatile Mtx, Mtxes...>;

	template<lockable_object Mtx, lockable_object... Mtxes>
	ScopedLock(Mtx&, volatile Mtxes&...) -> ScopedLock<Mtx, volatile Mtxes...>;

	template<lockable_object Mtx, lockable_object... Mtxes>
	ScopedLock(volatile Mtx&, volatile Mtxes&...) -> ScopedLock<volatile Mtx, volatile Mtxes...>;
}
