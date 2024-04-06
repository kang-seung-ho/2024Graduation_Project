module;
#include <mutex>
#include <shared_mutex>

export module Iconer.Utility.Concurrency.SharedMutex;
export import Iconer.Utility.Concurrency.Lock;

export namespace iconer::util
{
	class [[nodiscard]] SharedMutex
	{
	public:
		SharedMutex() noexcept = default;

		void LockWriter() noexcept
		{
			myImplement.lock();
		}

		void UnlckWriter() noexcept
		{
			myImplement.unlock();
		}

		void LockReader() noexcept
		{
			myImplement.lock_shared();
		}

		void UnlckReader() noexcept
		{
			myImplement.unlock_shared();
		}

		[[nodiscard]]
		bool TryLockWriter() noexcept
		{
			return myImplement.try_lock();
		}

		[[nodiscard]]
		bool TryLockReader() noexcept
		{
			return myImplement.try_lock_shared();
		}

		template<typename M>
		friend class std::lock_guard;
		template<typename... Mtxes>
		friend class std::scoped_lock;
		template<typename M>
		friend class std::unique_lock;
		template<typename M>
		friend class std::shared_lock;
		template<lockable_object... Mtxes>
		friend class ScopedLock;

	private:
		void lock() noexcept
		{
			myImplement.lock();
		}

		void unlock() noexcept
		{
			myImplement.unlock();
		}

		void lock_shared() noexcept
		{
			myImplement.lock_shared();
		}

		void unlock_shared() noexcept
		{
			myImplement.unlock_shared();
		}

		[[nodiscard]]
		bool try_lock() noexcept
		{
			return myImplement.try_lock();
		}

		[[nodiscard]]
		bool try_lock_shared() noexcept
		{
			return myImplement.try_lock_shared();
		}

		SharedMutex(const SharedMutex&) noexcept = delete;
		SharedMutex& operator=(const SharedMutex&) noexcept = delete;

		std::shared_mutex myImplement;
	};
}
