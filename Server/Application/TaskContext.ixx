module;
#include <concepts>

export module Iconer.App.TaskContext;
import Iconer.Net.IoContext;
export import Iconer.App.TaskCategory;
import <atomic>;

export namespace iconer::app
{
	class [[nodiscard]] TaskContext : public iconer::net::IoContext
	{
	public:
		using super = iconer::net::IoContext;
		using this_class = TaskContext;

		std::atomic<TaskCategory> myCategory;
		std::atomic_bool isOccupied;

		constexpr TaskContext() noexcept = default;
		constexpr ~TaskContext() noexcept = default;

		constexpr TaskContext(TaskCategory category) noexcept
			: myCategory(category)
			, isOccupied()
		{}

		void SetOperation(TaskCategory op, std::memory_order order = std::memory_order_seq_cst) noexcept
		{
			myCategory.store(op, order);
		}
		
		void SetOperation(TaskCategory op, std::memory_order order = std::memory_order_seq_cst) volatile noexcept
		{
			myCategory.store(op, order);
		}

		bool TryChangeOperation(TaskCategory from, TaskCategory to, std::memory_order order = std::memory_order_seq_cst) noexcept
		{
			return myCategory.compare_exchange_strong(from, to, order);
		}
		
		bool TryChangeOperation(TaskCategory from, TaskCategory to, std::memory_order order = std::memory_order_seq_cst) volatile noexcept
		{
			return myCategory.compare_exchange_strong(from, to, order);
		}

		template<std::invocable Fn>
		TaskContext& and_then(TaskCategory op, Fn&& fun, std::memory_order order = std::memory_order_acquire) noexcept(std::is_nothrow_invocable_v<Fn>)
		{
			auto prior = myCategory.load(order);

			while (true)
			{
				bool expected = false;

				if (isOccupied.compare_exchange_strong(expected, true, std::memory_order_acquire))
				{
					if (op == prior)
					{
						(std::forward<Fn>(fun))();
					}

					bool rollback = true;
					isOccupied.compare_exchange_strong(rollback, false, std::memory_order_release);

					break;
				}
			}
		}

		template<std::invocable Fn>
		TaskContext& or_else(TaskCategory op, Fn&& fun, std::memory_order order = std::memory_order_acquire) noexcept(std::is_nothrow_invocable_v<Fn>)
		{
			auto prior = myCategory.load(order);

			while (true)
			{
				bool expected = false;

				if (isOccupied.compare_exchange_strong(expected, true, std::memory_order_acquire))
				{
					if (op != prior)
					{
						(std::forward<Fn>(fun))();
					}

					bool rollback = true;
					isOccupied.compare_exchange_strong(rollback, false, std::memory_order_release);

					break;
				}
			}
		}
	};
}
