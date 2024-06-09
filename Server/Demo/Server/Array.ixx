module;
#include <stdexcept>

export module Iconer.Collection.Array;
export import :ArrayIterator;
import Iconer.Utility.Constraints;
import <memory>;
import <ranges>;
import <initializer_list>;

export namespace std
{
	template<typename T, size_t Size, typename Alloc1, typename Alloc2>
	constexpr void
		swap(iconer::collection::Array<T, Size, Alloc1>& lhs, iconer::collection::Array<T, Size, Alloc2>& rhs)
		noexcept;
}

export namespace iconer::collection
{
	template<typename T, size_t Size, typename Alloc>
	class [[nodiscard]] Array
	{
	public:
		static_assert(not std::is_reference_v<T>);

		using self_type = Array<T, Size, Alloc>;
		using allocator_type = Alloc;
		using allocator_trait = std::allocator_traits<allocator_type>;

		using value_type = allocator_trait::value_type;
		using size_type = allocator_trait::size_type;
		using difference_type = allocator_trait::difference_type;
		using pointer = allocator_trait::pointer;
		using const_pointer = allocator_trait::const_pointer;
		using reference = add_lvalue_reference_t<value_type>;
		using const_reference = add_lvalue_reference_t<add_const_t<value_type>>;
		using volatile_reference = add_lvalue_reference_t<add_volatile_t<value_type>>;
		using const_volatile_reference = add_lvalue_reference_t<add_volatile_t<add_const_t<value_type>>>;
		using rvalue_type = add_rvalue_reference_t<value_type>;
		using const_rvalue_type = add_rvalue_reference_t<add_const_t<value_type>>;
		using volatile_rvalue = add_rvalue_reference_t<add_volatile_t<value_type>>;
		using const_volatile_rvalue = add_rvalue_reference_t<add_volatile_t<add_const_t<value_type>>>;

		using const_iterator = ArrayConstIterator<T, Size>;
		using iterator = ArrayIterator<T, Size>;
		using volatile_iterator = ArrayIterator<T, Size>;
		using const_volatile_iterator = ArrayConstIterator<T, Size>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		static inline constexpr bool IsCopyConstructible = copy_constructibles<value_type>;
		static inline constexpr bool IsCopyAssignable = copy_assignables<value_type>;
		static inline constexpr bool IsMoveConstructible = move_constructibles<value_type>;
		static inline constexpr bool IsMoveAssignable = move_assignables<value_type>;

	private:
		explicit constexpr Array(std::in_place_t)
			: myData(nullptr), myAllocator()
		{
			myData = myAllocator.allocate(max_size());
		}

		explicit constexpr Array(std::in_place_t, const allocator_type& allocator)
			: myData(nullptr), myAllocator(allocator)
		{
			myData = myAllocator.allocate(max_size());
		}

	public:
		constexpr Array()
			: Array(std::in_place)
		{
			for (T* it = myData; it != myData + Size; ++it)
			{
				std::uninitialized_construct_using_allocator(it, myAllocator);
			}
		}

		constexpr Array(const allocator_type& allocator)
			: Array(std::in_place)
		{
			for (T* it = myData; it != myData + Size; ++it)
			{
				std::uninitialized_construct_using_allocator(it, myAllocator);
			}
		}

		constexpr ~Array()
		{
			for (T* it = myData; it != myData + Size; ++it)
			{
				std::destroy_at(it);
			}

			myAllocator.deallocate(myData, Size);
		}

		explicit constexpr Array(std::initializer_list<value_type> list) requires copyable<value_type>
			: Array(std::in_place)
		{
			for (auto it = begin(); const value_type & item : list)
			{
				if (it == end()) break;

				std::uninitialized_construct_using_allocator(it, myAllocator, item);
				(void)++it;
			}
		}

		template<typename U, size_t L>
		explicit constexpr Array(const U(&array)[L]) requires copyable<value_type> and copyable<U>
			: Array(std::in_place)
		{
			for (auto it = begin(); const U & item : array)
			{
				if (it == end()) break;

				std::uninitialized_construct_using_allocator(it, myAllocator, item);
				(void)++it;
			}
		}

		template<typename U, size_t L>
		explicit constexpr Array(U(&& array)[L]) requires movable<value_type> and movable<U>
			: Array(std::in_place)
		{
			for (auto it = begin(); U & item : array)
			{
				if (it == end()) break;

				std::uninitialized_construct_using_allocator(it, myAllocator, std::move(item));
				(void)++it;
			}
		}

		template<std::ranges::range R>
		explicit constexpr Array(std::from_range_t, R&& range)
			: Array(std::in_place)
		{
			for (auto it = begin(); auto && item : std::forward<R>(range))
			{
				if (it == end()) break;

				std::uninitialized_construct_using_allocator(it, myAllocator, std::forward<std::ranges::range_value_t<R>>(item));
				(void)++it;
			}
		}

		template<std::input_iterator It, std::sentinel_for<It> Sentinel>
		explicit constexpr Array(It it, const Sentinel sentinel)
			: Array(std::in_place)
		{
			auto mit = begin();
			for (; it != sentinel; ++it)
			{
				if (mit == end()) break;

				std::uninitialized_construct_using_allocator(it, myAllocator, std::move(*it));
				(void)++it;
			}
		}

		constexpr Array(const Array& other, allocator_type allocator = {}) requires IsCopyConstructible
			: Array(std::in_place, allocator)
		{
			auto mit = begin();
			for (auto it = other.cbegin(); it != other.cend(); ++it)
			{
				if (mit == end()) break;

				std::uninitialized_construct_using_allocator(it, myAllocator, std::as_const(*it));
				(void)++it;
			}
		}

		constexpr Array(const volatile Array& other, allocator_type allocator = {}) requires IsCopyConstructible
			: Array(std::in_place, allocator)
		{
			auto mit = begin();
			for (auto it = other.cbegin(); it != other.cend(); ++it)
			{
				if (mit == end()) break;

				std::uninitialized_construct_using_allocator(it, myAllocator, std::as_const(*it));
				(void)++it;
			}
		}

		constexpr Array& operator=(const Array& other)
			requires IsCopyAssignable
		{
			auto mit = begin();
			for (auto it = other.cbegin(); it != other.cend(); ++it)
			{
				if (mit == end()) break;
				*mit = *it;

				(void)++it;
			}

			return *this;
		}

		constexpr Array& operator=(const volatile Array& other)
			requires IsCopyAssignable
		{
			auto mit = begin();
			for (auto it = other.cbegin(); it != other.cend(); ++it)
			{
				if (mit == end()) break;
				*mit++ = *it;

				(void)++it;
			}

			return *this;
		}

		constexpr volatile Array& operator=(const Array& other)
			volatile requires IsCopyAssignable
		{
			auto mit = begin();
			for (auto it = other.cbegin(); it != other.cend(); ++it)
			{
				if (mit == end()) break;
				*mit++ = *it;

				(void)++it;
			}

			return *this;
		}

		constexpr volatile Array& operator=(const volatile Array& other)
			volatile requires IsCopyAssignable
		{
			auto mit = begin();
			for (auto it = other.cbegin(); it != other.cend(); ++it)
			{
				if (mit == end()) break;
				*mit++ = *it;

				(void)++it;
			}

			return *this;
		}

		constexpr Array(Array&& other, allocator_type allocator = {}) noexcept
			: myData(std::exchange(other.myData, nullptr))
			, myAllocator(allocator)
		{
		}

		constexpr Array(volatile Array&& other, allocator_type allocator = {}) noexcept
			: myData(std::exchange(other.myData, nullptr))
			, myAllocator(allocator)
		{
		}

		constexpr Array& operator=(Array&& other) noexcept
		{
			myData = std::exchange(other.myData, nullptr);

			return *this;
		}

		constexpr Array& operator=(volatile Array&& other) noexcept
		{
			myData = std::exchange(other.myData, nullptr);

			return *this;
		}

		constexpr volatile Array& operator=(Array&& other) volatile noexcept
		{
			myData = std::exchange(other.myData, nullptr);

			return *this;
		}

		constexpr volatile Array& operator=(volatile Array&& other) volatile noexcept
		{
			myData = std::exchange(other.myData, nullptr);

			return *this;
		}

		template<invocables<reference> Predicate>
		constexpr void ForEach(Predicate&& pred)
			noexcept(nothrow_invocables<Predicate, reference>)
		{
			for (auto it = begin(); it != end(); ++it)
			{
				std::invoke(std::forward<Predicate>(pred), *it);
			}
		}

		template<invocables<reference> Predicate>
		constexpr void ForEach(Predicate&& pred)
			const noexcept(nothrow_invocables<Predicate, const_reference>)
		{
			for (auto it = begin(); it != end(); ++it)
			{
				std::invoke(std::forward<Predicate>(pred), *it);
			}
		}

		template<invocables<reference> Predicate>
		constexpr void ForEach(Predicate&& pred)
			volatile noexcept(nothrow_invocables<Predicate, volatile_reference>)
		{
			for (auto it = begin(); it != end(); ++it)
			{
				std::invoke(std::forward<Predicate>(pred), *it);
			}
		}

		template<invocables<reference> Predicate>
		constexpr void ForEach(Predicate&& pred)
			const volatile noexcept(nothrow_invocables<Predicate, const_volatile_reference>)
		{
			for (auto it = begin(); it != end(); ++it)
			{
				std::invoke(std::forward<Predicate>(pred), *it);
			}
		}

		[[nodiscard]]
		constexpr reference At(size_type index)&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return myData[index];
		}

		[[nodiscard]]
		constexpr const_reference At(size_type index) const&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return myData[index];
		}

		[[nodiscard]]
		constexpr volatile_reference At(size_type index) volatile&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return myData[index];
		}

		[[nodiscard]]
		constexpr const_volatile_reference At(size_type index) const volatile&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return myData[index];
		}

		[[nodiscard]]
		constexpr rvalue_type At(size_type index)&&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return std::move(myData[index]);
		}

		[[nodiscard]]
		constexpr const_rvalue_type At(size_type index) const&&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return std::move(myData[index]);
		}

		[[nodiscard]]
		constexpr volatile_rvalue At(size_type index) volatile&&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return std::move(myData[index]);
		}

		[[nodiscard]]
		constexpr const_volatile_rvalue At(size_type index) const volatile&&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return std::move(myData[index]);
		}

		[[nodiscard]]
		constexpr reference operator[](size_type index)&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return myData[index];
		}

		[[nodiscard]]
		constexpr const_reference operator[](size_type index) const&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return myData[index];
		}

		[[nodiscard]]
		constexpr volatile_reference operator[](size_type index) volatile&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return myData[index];
		}

		[[nodiscard]]
		constexpr const_volatile_reference operator[](size_type index) const volatile&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return myData[index];
		}

		[[nodiscard]]
		constexpr rvalue_type operator[](size_type index)&&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return std::move(myData[index]);
		}

		[[nodiscard]]
		constexpr const_rvalue_type operator[](size_type index) const&&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return std::move(myData[index]);
		}

		[[nodiscard]]
		constexpr volatile_rvalue operator[](size_type index) volatile&&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return std::move(myData[index]);
		}

		[[nodiscard]]
		constexpr const_volatile_rvalue operator[](size_type index) const volatile&&
		{
			if (Size <= index)
			{
				ThrowAccessError();
			}

			return std::move(myData[index]);
		}

		template<typename U>
		[[nodiscard]]
		constexpr value_type GetOr(size_type index, U&& alternative) const
		{
			if (Size <= index)
			{
				return std::forward<U>(alternative);
			}
			else
			{
				return myData[index];
			}
		}

		template<typename U>
		[[nodiscard]]
		constexpr value_type GetOr(size_type index, U&& alternative) const volatile
		{
			if (Size <= index)
			{
				return std::forward<U>(alternative);
			}
			else
			{
				return myData[index];
			}
		}

		[[nodiscard]]
		constexpr iterator begin() noexcept
		{
			return { myData };
		}

		[[nodiscard]]
		constexpr iterator end() noexcept
		{
			return { myData + Size };
		}

		[[nodiscard]]
		constexpr const_iterator begin() const noexcept
		{
			return { myData };
		}

		[[nodiscard]]
		constexpr const_iterator end() const noexcept
		{
			return { myData + Size };
		}

		[[nodiscard]]
		constexpr const_iterator cbegin() const noexcept
		{
			return { myData };
		}

		[[nodiscard]]
		constexpr const_iterator cend() const noexcept
		{
			return { myData + Size };
		}

		[[nodiscard]]
		constexpr volatile_iterator begin() volatile noexcept
		{
			return { myData };
		}

		[[nodiscard]]
		constexpr volatile_iterator end() volatile noexcept
		{
			return { myData + Size };
		}

		[[nodiscard]]
		constexpr const_volatile_iterator begin() const volatile noexcept
		{
			return { myData };
		}

		[[nodiscard]]
		constexpr const_volatile_iterator end() const volatile noexcept
		{
			return { myData + Size };
		}

		[[nodiscard]]
		constexpr const_volatile_iterator cbegin() const volatile noexcept
		{
			return { myData };
		}

		[[nodiscard]]
		constexpr const_volatile_iterator cend() const volatile noexcept
		{
			return { myData + Size };
		}

		[[nodiscard]]
		constexpr size_type size() const noexcept
		{
			return Size;
		}

		[[nodiscard]]
		constexpr size_type size() const volatile noexcept
		{
			return Size;
		}

		[[nodiscard]]
		constexpr difference_type ssize() const noexcept
		{
			return static_cast<difference_type>(Size);
		}

		[[nodiscard]]
		constexpr difference_type ssize() const volatile noexcept
		{
			return static_cast<difference_type>(Size);
		}

		[[nodiscard]]
		static consteval size_type max_size() noexcept
		{
			return Size;
		}

		[[nodiscard]]
		constexpr bool operator==(const Array& other) const noexcept
		{
			if (std::addressof(other) == this) return true;
			if (other.myData == myData) return true;

			auto mit = cbegin();
			for (auto it = other.cbegin(); it != other.cend(); (void)++it)
			{
				if (mit == cend()) break;
				if (*mit != *it)
				{
					return false;
				}

				(void)++mit;
			}

			return true;
		}

		[[nodiscard]]
		constexpr bool operator==(const Array& other) const volatile noexcept
		{
			if (std::addressof(other) == this) return true;
			if (other.myData == myData) return true;

			auto mit = cbegin();
			for (auto it = other.cbegin(); it != other.cend(); (void)++it)
			{
				if (mit == cend()) break;
				if (*mit != *it)
				{
					return false;
				}

				(void)++mit;
			}

			return true;
		}

		[[nodiscard]]
		constexpr bool operator==(const volatile Array& other) const noexcept
		{
			if (std::addressof(other) == this) return true;
			if (other.myData == myData) return true;

			auto mit = cbegin();
			for (auto it = other.cbegin(); it != other.cend(); (void)++it)
			{
				if (mit == cend()) break;
				if (*mit != *it)
				{
					return false;
				}

				(void)++mit;
			}

			return true;
		}

		[[nodiscard]]
		constexpr bool operator==(const volatile Array& other) const volatile noexcept
		{
			if (std::addressof(other) == this) return true;
			if (other.myData == myData) return true;

			auto mit = cbegin();
			for (auto it = other.cbegin(); it != other.cend(); (void)++it)
			{
				if (mit == cend()) break;
				if (*mit != *it)
				{
					return false;
				}

				(void)++mit;
			}

			return true;
		}

		template<typename T, size_t Size, typename Alloc1, typename Alloc2>
		friend constexpr void std::swap(Array<T, Size, Alloc1>& lhs, Array<T, Size, Alloc2>& rhs) noexcept;

	private:
		[[noreturn]]
		static void ThrowAccessError()
		{
			throw std::out_of_range{ "Invalid Array<T, Size> subscript!" };
		}

		T* myData;
		allocator_type myAllocator;
	};

	template<typename T, size_t Size>
	Array(T[Size]) -> Array<T, Size>;
	template<typename T, size_t Size>
	Array(std::array<T, Size>) -> Array<T, Size>;
}

export namespace std
{
	template<typename T, size_t Size, typename Alloc1, typename Alloc2>
	constexpr void
		swap(iconer::collection::Array<T, Size, Alloc1>& lhs, iconer::collection::Array<T, Size, Alloc2>& rhs)
		noexcept
	{
		using ::std::swap;
		swap(lhs.myData, rhs.myData);
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto begin(iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.begin();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto end(iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.end();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto cbegin(iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.begin();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto cend(iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.end();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto begin(const iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.begin();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto end(const iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.end();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto cbegin(const iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.begin();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto cend(const iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.end();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto begin(volatile iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.begin();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto end(volatile iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.end();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto cbegin(volatile iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.begin();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto cend(volatile iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.end();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto begin(const volatile iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.begin();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto end(const volatile iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.end();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto cbegin(const volatile iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.begin();
	}

	template<typename T, size_t Size, typename Alloc>
	[[nodiscard]]
	constexpr auto cend(const volatile iconer::collection::Array<T, Size, Alloc>& arr) noexcept
	{
		return arr.end();
	}
}

template<typename T, size_t Size, typename Alloc>
inline constexpr bool std::ranges::enable_borrowed_range<iconer::collection::Array<T, Size, Alloc>> = true;

module :private;

namespace iconer::collection::test
{
	constexpr void testments()
	{
		Array<int, 100> arr0{};
		arr0.At(0);

		volatile Array<int, 100> arr1{};
		arr1.At(0);

		const Array<int, 100> arr2{};
		arr2.At(0);

		const volatile Array<int, 100> arr3{};
		arr3.At(0);
	}
}
