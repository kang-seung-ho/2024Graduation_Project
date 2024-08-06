module;
#include <memory>
#include <algorithm>

export module Iconer.Collection.Array:ArrayIterator;
import Iconer.Utility.Constraints;

export namespace iconer::collection
{
	template<typename T, size_t Size, typename Alloc = std::allocator<T>>
	class [[nodiscard]] Array;
}

export namespace iconer::collection
{
	template<typename T, size_t Size>
	class [[nodiscard]] ArrayConstIterator
	{
	public:
		using iterator_concept = std::contiguous_iterator_tag;
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = const T*;
		using reference = const T&;

		constexpr ArrayConstIterator() noexcept = default;

		constexpr ArrayConstIterator& operator++() noexcept
		{
			(void)++dataHandle;
			return *this;
		}

		constexpr volatile ArrayConstIterator& operator++() volatile noexcept
		{
			(void)++dataHandle;
			return *this;
		}

		constexpr ArrayConstIterator operator++(int) noexcept
		{
			return ArrayConstIterator{ ++dataHandle };
		}

		constexpr ArrayConstIterator operator++(int) volatile noexcept
		{
			return ArrayConstIterator{ ++dataHandle };
		}

		constexpr ArrayConstIterator& operator=(T* ptr) noexcept
		{
			dataHandle = ptr;
			return *this;
		}

		constexpr volatile ArrayConstIterator& operator=(T* ptr) volatile noexcept
		{
			dataHandle = ptr;
			return *this;
		}

		constexpr pointer operator->() const noexcept
		{
			return dataHandle;
		}

		constexpr pointer operator->() const volatile noexcept
		{
			return dataHandle;
		}

		[[nodiscard]]
		constexpr reference operator*() const noexcept
		{
			return *dataHandle;
		}

		[[nodiscard]]
		constexpr reference operator*() const volatile noexcept
		{
			return *dataHandle;
		}

		[[nodiscard]]
		friend constexpr bool
			operator==(const ArrayConstIterator& lhs, const ArrayConstIterator& rhs)
			noexcept
		{
			return lhs.dataHandle == rhs.dataHandle;
		}

		[[nodiscard]]
		friend constexpr bool
			operator==(const volatile ArrayConstIterator& lhs, const ArrayConstIterator& rhs)
			noexcept
		{
			return lhs.dataHandle == rhs.dataHandle;
		}

		[[nodiscard]]
		friend constexpr bool
			operator==(const ArrayConstIterator& lhs, const volatile ArrayConstIterator& rhs)
			noexcept
		{
			return lhs.dataHandle == rhs.dataHandle;
		}

		[[nodiscard]]
		friend constexpr bool
			operator==(const volatile ArrayConstIterator& lhs, const volatile ArrayConstIterator& rhs)
			noexcept
		{
			return lhs.dataHandle == rhs.dataHandle;
		}

		[[nodiscard]]
		friend constexpr std::strong_ordering
			operator<=>(const ArrayConstIterator& lhs, const ArrayConstIterator& rhs)
			noexcept
		{
			return lhs.dataHandle <=> rhs.dataHandle;
		}

		[[nodiscard]]
		friend constexpr std::strong_ordering
			operator<=>(const ArrayConstIterator& lhs, const volatile ArrayConstIterator& rhs)
			noexcept
		{
			return lhs.dataHandle <=> rhs.dataHandle;
		}

		[[nodiscard]]
		friend constexpr std::strong_ordering
			operator<=>(const volatile ArrayConstIterator& lhs, const ArrayConstIterator& rhs)
			noexcept
		{
			return lhs.dataHandle <=> rhs.dataHandle;
		}

		[[nodiscard]]
		friend constexpr std::strong_ordering
			operator<=>(const volatile ArrayConstIterator& lhs, const volatile ArrayConstIterator& rhs)
			noexcept
		{
			return lhs.dataHandle <=> rhs.dataHandle;
		}

		template<typename T, size_t Size, typename Alloc>
		friend class Array;

	protected:
		constexpr ArrayConstIterator(T* ptr) noexcept
			: dataHandle(ptr)
		{
		}

		T* dataHandle;
	};

	template<typename T, size_t Size>
	class [[nodiscard]] ArrayIterator : public ArrayConstIterator<T, Size>
	{
	public:
		using iterator_concept = std::contiguous_iterator_tag;
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;

		using Super = ArrayConstIterator<T, Size>;
		using Super::Super;

		constexpr ArrayIterator& operator++() noexcept
		{
			Super::operator++();
			return *this;
		}

		constexpr volatile ArrayIterator& operator++() volatile noexcept
		{
			Super::operator++();
			return *this;
		}

		constexpr ArrayIterator operator++(int) noexcept
		{
			return ArrayIterator{ Super::dataHandle++ };
		}

		constexpr ArrayIterator operator++(int) volatile noexcept
		{
			return ArrayIterator{ Super::dataHandle++ };
		}

		constexpr ArrayIterator& operator=(T* ptr) noexcept
		{
			static_cast<Super&>(*this).operator=(ptr);
			return *this;
		}

		constexpr volatile ArrayIterator& operator=(T* ptr) volatile noexcept
		{
			static_cast<volatile Super&>(*this).operator=(ptr);
			return *this;
		}

		constexpr pointer operator->() noexcept
		{
			return Super::dataHandle;
		}

		constexpr pointer operator->() const noexcept
		{
			return Super::dataHandle;
		}

		constexpr pointer operator->() volatile noexcept
		{
			return Super::dataHandle;
		}

		constexpr pointer operator->() const volatile noexcept
		{
			return Super::dataHandle;
		}

		[[nodiscard]]
		constexpr reference operator*() noexcept
		{
			return *Super::dataHandle;
		}

		[[nodiscard]]
		constexpr reference operator*() const noexcept
		{
			return *Super::dataHandle;
		}

		[[nodiscard]]
		constexpr reference operator*() volatile noexcept
		{
			return *Super::dataHandle;
		}

		[[nodiscard]]
		constexpr reference operator*() const volatile noexcept
		{
			return *Super::dataHandle;
		}
	};
}
