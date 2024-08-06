// Copyright (c) 2019 Maxim Egorushkin. MIT License. See the full licence in file LICENSE.
#pragma once
#include <CoreMinimal.h>
#include <atomic>
#include <emmintrin.h>

#define ATOMIC_QUEUE_LIKELY(expr) (expr)
#define ATOMIC_QUEUE_UNLIKELY(expr) (expr)
#define ATOMIC_QUEUE_NOINLINE

namespace atomic_queue
{
	constexpr int CACHE_LINE_SIZE = 64;

	static void spin_loop_pause() noexcept
	{
		_mm_pause();
	}

	auto inline constexpr A = std::memory_order_acquire;
	auto inline constexpr R = std::memory_order_release;
	auto inline constexpr X = std::memory_order_relaxed;
	auto inline constexpr C = std::memory_order_seq_cst;
	auto inline constexpr AR = std::memory_order_acq_rel;

} // namespace atomic_queue
