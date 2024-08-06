export module Iconer.Utility.MovableAtomic;
import Iconer.Utility.TypeTraits;
import Iconer.Utility.Atomic;
import <compare>;
import <utility>;

export namespace iconer::util
{
	template<typename T>
	class MovableAtomic 
	{
	public:
		using value_type = std::atomic<T>;
		using integral_type = T;

		static inline constexpr bool IsNothrowReadable = nothrow_atomic_readable<value_type>;
		static inline constexpr bool IsNothrowWritable = nothrow_atomic_writable<value_type>;

		explicit constexpr MovableAtomic() noexcept(nothrow_default_constructibles<value_type>) = default;
		constexpr ~MovableAtomic() noexcept(nothrow_destructibles<value_type>) = default;

		explicit MovableAtomic(const integral_type& value)
			noexcept(IsNothrowWritable)
			: myValue()
		{
			myValue.store(value, std::memory_order::relaxed);
		}

		explicit MovableAtomic(integral_type&& value)
			noexcept(IsNothrowWritable)
			: myValue()
		{
			myValue.store(static_cast<integral_type&&>(value), std::memory_order::relaxed);
		}

		explicit MovableAtomic(MovableAtomic&& other)
			noexcept(IsNothrowReadable and IsNothrowWritable)
			: myValue()
		{
			myValue.store(other.Load(std::memory_order::relaxed), std::memory_order::relaxed);
		}

		template<typename U>
			requires requires{ std::declval<value_type>().store(std::declval<U>()); }
		void Store(U&& value, std::memory_order order = std::memory_order::seq_cst)
			noexcept(IsNothrowWritable)
		{
			myValue.store(std::forward<U>(value), order);
		}

		template<typename U>
			requires requires{ std::declval<value_type>().store(std::declval<U>()); }
		void Store(U&& value, std::memory_order order = std::memory_order::seq_cst)
			volatile noexcept(IsNothrowWritable)
		{
			myValue.store(std::forward<U>(value), order);
		}

		[[nodiscard]]
		integral_type Load(std::memory_order order = std::memory_order::seq_cst)
			const noexcept(IsNothrowReadable)
		{
			return myValue.load(order);
		}

		[[nodiscard]]
		integral_type Load(std::memory_order order = std::memory_order::seq_cst)
			const volatile noexcept(IsNothrowReadable)
		{
			return myValue.load(order);
		}

		integral_type FetchAdd(const integral_type& operand, const std::memory_order order = std::memory_order_seq_cst) noexcept
		{
			return myValue.fetch_add(operand, order);
		}

		integral_type FetchSubtract(const integral_type& operand, const std::memory_order order = std::memory_order_seq_cst) noexcept
		{
			return myValue.fetch_sub(operand, order);
		}

		integral_type FetchAnd(const integral_type& operand, const std::memory_order order = std::memory_order_seq_cst) noexcept
		{
			return myValue.fetch_and(operand, order);
		}

		integral_type FetchOr(const integral_type& operand, const std::memory_order order = std::memory_order_seq_cst) noexcept
		{
			return myValue.fetch_or(operand, order);
		}

		integral_type FetchXor(const integral_type& operand, const std::memory_order order = std::memory_order_seq_cst) noexcept
		{
			return myValue.fetch_xor(operand, order);
		}

		integral_type FetchAdd(integral_type&& operand, const std::memory_order order = std::memory_order_seq_cst) noexcept
		{
			return myValue.fetch_add(std::move(operand), order);
		}

		integral_type FetchSubtract(integral_type&& operand, const std::memory_order order = std::memory_order_seq_cst) noexcept
		{
			return myValue.fetch_sub(std::move(operand), order);
		}

		integral_type FetchAnd(integral_type&& operand, const std::memory_order order = std::memory_order_seq_cst) noexcept
		{
			return myValue.fetch_and(std::move(operand), order);
		}

		integral_type FetchOr(integral_type&& operand, const std::memory_order order = std::memory_order_seq_cst) noexcept
		{
			return myValue.fetch_or(std::move(operand), order);
		}

		integral_type FetchXor(integral_type&& operand, const std::memory_order order = std::memory_order_seq_cst) noexcept
		{
			return myValue.fetch_xor(std::move(operand), order);
		}

		template<typename U> requires assignable_from<integral_type, U>
		integral_type FetchAdd(U&& operand, const std::memory_order order = std::memory_order_seq_cst)
			noexcept
		{
			return myValue.fetch_add(operand, std::forward<U>(order));
		}

		template<typename U> requires assignable_from<integral_type, U>
		integral_type FetchSubtract(U&& operand, const std::memory_order order = std::memory_order_seq_cst)
			noexcept
		{
			return myValue.fetch_sub(operand, std::forward<U>(order));
		}

		template<typename U> requires assignable_from<integral_type, U>
		integral_type FetchAnd(U&& operand, const std::memory_order order = std::memory_order_seq_cst)
			noexcept
		{
			return myValue.fetch_and(operand, std::forward<U>(order));
		}

		template<typename U> requires assignable_from<integral_type, U>
		integral_type FetchOr(U&& operand, const std::memory_order order = std::memory_order_seq_cst)
			noexcept
		{
			return myValue.fetch_or(operand, std::forward<U>(order));
		}

		template<typename U> requires assignable_from<integral_type, U>
		integral_type FetchXor(U&& operand, const std::memory_order order = std::memory_order_seq_cst)
			noexcept
		{
			return myValue.fetch_xor(operand, std::forward<U>(order));
		}

		template<typename U>
			requires requires{ std::declval<value_type>().store(std::declval<U>()); }
		bool CompareAndSet(integral_type expected, U&& value)
			noexcept
		{
			return myValue.compare_exchange_strong(expected, std::forward<U>(value));
		}

		template<typename U>
			requires requires{ std::declval<value_type>().store(std::declval<U>()); }
		bool CompareAndSet(integral_type expected, U&& value)
			volatile noexcept
		{
			return myValue.compare_exchange_strong(expected, std::forward<U>(value));
		}

		template<typename U>
			requires requires{ std::declval<value_type>().store(std::declval<U>()); }
		bool CompareAndSet(integral_type expected, U&& value, std::memory_order order)
			noexcept
		{
			return myValue.compare_exchange_strong(expected, std::forward<U>(value), order);
		}

		template<typename U>
			requires requires{ std::declval<value_type>().store(std::declval<U>()); }
		bool CompareAndSet(integral_type expected, U&& value, std::memory_order order)
			volatile noexcept
		{
			return myValue.compare_exchange_strong(expected, std::forward<U>(value), order);
		}

		template<typename U>
			requires requires{ std::declval<value_type>().store(std::declval<U>()); }
		bool CompareAndSet(integral_type expected, U&& value, std::memory_order success, std::memory_order failure)
			noexcept
		{
			return myValue.compare_exchange_strong(expected, std::forward<U>(value), success, failure);
		}

		template<typename U>
			requires requires{ std::declval<value_type>().store(std::declval<U>()); }
		bool CompareAndSet(integral_type expected, U&& value, std::memory_order success, std::memory_order failure)
			volatile noexcept
		{
			return myValue.compare_exchange_strong(expected, std::forward<U>(value), success, failure);
		}

		template<typename U>
			requires requires{ std::declval<value_type>().store(std::declval<U>()); }
		integral_type Exchange(U&& value, std::memory_order order = std::memory_order::relaxed)
			noexcept
		{
			return std::atomic_exchange_explicit(std::addressof(myValue), std::forward<U>(value), order);
		}

		template<typename U>
			requires requires{ std::declval<value_type>().store(std::declval<U>()); }
		integral_type Exchange(U&& value, std::memory_order order = std::memory_order::relaxed)
			volatile noexcept
		{
			return std::atomic_exchange_explicit(std::addressof(myValue), std::forward<U>(value), order);
		}

		[[nodiscard]]
		bool Equals(const integral_type& value, std::memory_order order = std::memory_order::relaxed)
			const noexcept(IsNothrowReadable)
		{
			return value == myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const integral_type& value, std::memory_order order = std::memory_order::relaxed)
			const volatile noexcept(IsNothrowReadable)
		{
			return value == myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const value_type& atom, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const noexcept(IsNothrowReadable)
		{
			return atom.load(rhs_order) == myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const volatile value_type& atom, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const noexcept(IsNothrowReadable)
		{
			return atom.load(rhs_order) == myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const value_type& atom, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const volatile noexcept(IsNothrowReadable)
		{
			return atom.load(rhs_order) == myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const volatile value_type& atom, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const volatile noexcept(IsNothrowReadable)
		{
			return atom.load(rhs_order) == myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const MovableAtomic& other, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const noexcept(IsNothrowReadable)
		{
			return other.myValue.load(rhs_order) == myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const volatile MovableAtomic& other, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const noexcept(IsNothrowReadable)
		{
			return other.myValue.load(rhs_order) == myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const MovableAtomic& other, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const volatile noexcept(IsNothrowReadable)
		{
			return other.myValue.load(rhs_order) == myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const volatile MovableAtomic& other, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const volatile noexcept(IsNothrowReadable)
		{
			return other.myValue.load(rhs_order) == myValue.load(order);
		}

		MovableAtomic& operator=(const integral_type& value)
			noexcept(IsNothrowWritable)
		{
			myValue.store(value);

			return *this;
		}

		MovableAtomic& operator=(integral_type&& value)
			noexcept(IsNothrowWritable)
		{
			myValue.store(static_cast<integral_type&&>(value));

			return *this;
		}

		volatile MovableAtomic& operator=(const integral_type& value)
			volatile noexcept(IsNothrowWritable)
		{
			myValue.store(value);

			return *this;
		}

		volatile MovableAtomic& operator=(integral_type&& value)
			volatile noexcept(IsNothrowWritable)
		{
			myValue.store(static_cast<integral_type&&>(value));

			return *this;
		}

		MovableAtomic& operator=(const value_type& atom)
			noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(atom.load(std::memory_order::relaxed));

			return *this;
		}

		MovableAtomic& operator=(value_type&& atom)
			noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(atom.load(std::memory_order::relaxed));

			return *this;
		}

		volatile MovableAtomic& operator=(const value_type& atom)
			volatile noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(atom.load(std::memory_order::relaxed));

			return *this;
		}

		volatile MovableAtomic& operator=(value_type&& atom)
			volatile noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(atom.load(std::memory_order::relaxed));

			return *this;
		}

		MovableAtomic& operator=(MovableAtomic&& other)
			noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(other.myValue.load(std::memory_order::relaxed));

			return *this;
		}

		MovableAtomic& operator=(volatile MovableAtomic&& other)
			noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(other.myValue.load(std::memory_order::relaxed));

			return *this;
		}

		volatile MovableAtomic& operator=(MovableAtomic&& other)
			volatile noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(other.myValue.load(std::memory_order::relaxed));

			return *this;
		}

		volatile MovableAtomic& operator=(volatile MovableAtomic&& other)
			volatile noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(other.myValue.load(std::memory_order::relaxed));

			return *this;
		}

		[[nodiscard]]
		operator integral_type() const noexcept
		{
			return myValue.load(std::memory_order::relaxed);
		}

		[[nodiscard]]
		operator integral_type() const volatile noexcept
		{
			return myValue.load(std::memory_order::relaxed);
		}

	protected:
		value_type myValue;

	private:
		MovableAtomic(const MovableAtomic&) = delete;
		MovableAtomic& operator=(const MovableAtomic&) = delete;
	};
}

export namespace std
{
	template<typename T, typename U>
	T exchange(iconer::util::MovableAtomic<T>& lhs, U&& rhs, std::memory_order init_order = std::memory_order::acquire, std::memory_order final_order = std::memory_order::release)
		noexcept(is_nothrow_convertible_v<T, U&&>)
	{
		static_assert(is_convertible_v<T, U&&>);

		auto lval = lhs.Load(init_order);
		lhs.Store(std::forward<U>(rhs), final_order);
		return std::move(lval);
	}
}
