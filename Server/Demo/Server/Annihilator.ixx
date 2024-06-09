module;
#include <memory>
#include <utility>
export module Iconer.Utility.Annihilator;

export namespace iconer::util
{
	template<typename Deallocator>
	struct Annihilator
	{
		constexpr ~Annihilator() noexcept
		{
			if (ptrHandle != nullptr)
			{
				std::destroy_n(ptrHandle, ptrSize);

				Deallocator allocator{};
				allocator.deallocate(ptrHandle, ptrSize);
			}
		}

		std::allocator_traits<Deallocator>::pointer ptrHandle;
		size_t ptrSize;
	};

	template<typename Deallocator>
	struct FailsafeAnnihilator
	{
		constexpr ~FailsafeAnnihilator()
			noexcept(noexcept(std::declval<Deallocator>().deallocate(std::declval<std::allocator_traits<Deallocator>::pointer>(), size_t{})))
		{
			if (not isSafe and ptrHandle != nullptr)
			{
				std::destroy_n(ptrHandle, ptrSize);

				Deallocator allocator{};
				allocator.deallocate(ptrHandle, ptrSize);
			}
		}

		constexpr void MakeSafe() noexcept
		{
			isSafe = true;
		}

		constexpr void MakeSafe() volatile noexcept
		{
			isSafe = true;
		}

		std::allocator_traits<Deallocator>::pointer ptrHandle;
		size_t ptrSize;
		volatile bool isSafe;
	};
}
