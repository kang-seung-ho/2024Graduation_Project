// Copyright (c) 2019 Maxim Egorushkin. MIT License. See the full licence in file LICENSE.
#pragma once
#include "defs.h"

namespace atomic_queue
{
	class Barrier
	{
	public:
		void wait() noexcept
		{
			counter_.fetch_add(1, std::memory_order_acquire);

			while (0 < counter_.load(std::memory_order_relaxed))
			{
				spin_loop_pause();
			}
		}

		void release(unsigned expected_counter) noexcept
		{
			while (expected_counter != counter_.load(std::memory_order_relaxed))
			{
				spin_loop_pause();
			}

			counter_.store(0, std::memory_order_release);
		}

	private:
		std::atomic<unsigned> counter_;
	};
} // namespace atomic_queue

