export module Iconer.Utility.MovableAtomic:Trivial;
import Iconer.Utility.Constraints;
import Iconer.Utility.Atomic;
import :Storage;
import <compare>;
import <utility>;

namespace iconer::util
{
	template<typename T>
	class MovableAtomicImplTrivial : public MovableAtomicStorage<std::atomic<T>>
	{
	public:
		using Super = MovableAtomicStorage<std::atomic<T>>;
		using value_type = Super::value_type;
		using integral_type = Super::integral_type;

		static inline constexpr bool IsNothrowReadable = nothrow_atomic_readable<value_type>;
		static inline constexpr bool IsNothrowWritable = nothrow_atomic_writable<value_type>;

		explicit constexpr MovableAtomicImplTrivial() noexcept(nothrow_default_constructibles<value_type>) = default;
		constexpr ~MovableAtomicImplTrivial() noexcept(nothrow_destructibles<value_type>) = default;

		explicit MovableAtomicImplTrivial(const integral_type& value)
			noexcept(IsNothrowWritable)
			: myValue()
		{
			myValue.store(value, std::memory_order::relaxed);
		}

		explicit MovableAtomicImplTrivial(integral_type&& value)
			noexcept(IsNothrowWritable)
			: myValue()
		{
			myValue.store(static_cast<integral_type&&>(value), std::memory_order::relaxed);
		}

		explicit MovableAtomicImplTrivial(MovableAtomicImplTrivial&& other)
			noexcept(IsNothrowReadable and IsNothrowWritable)
			: myValue()
		{
			myValue.store(other.Load(std::memory_order::relaxed), std::memory_order::relaxed);
		}

		MovableAtomicImplTrivial& operator=(const integral_type& value)
			noexcept(IsNothrowWritable)
		{
			myValue.store(value);

			return *this;
		}

		MovableAtomicImplTrivial& operator=(integral_type&& value)
			noexcept(IsNothrowWritable)
		{
			myValue.store(static_cast<integral_type&&>(value));

			return *this;
		}

		volatile MovableAtomicImplTrivial& operator=(const integral_type& value)
			volatile noexcept(IsNothrowWritable)
		{
			myValue.store(value);

			return *this;
		}

		volatile MovableAtomicImplTrivial& operator=(integral_type&& value)
			volatile noexcept(IsNothrowWritable)
		{
			myValue.store(static_cast<integral_type&&>(value));

			return *this;
		}

		MovableAtomicImplTrivial& operator=(const value_type& atom)
			noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(atom.load(std::memory_order::relaxed));

			return *this;
		}

		MovableAtomicImplTrivial& operator=(value_type&& atom)
			noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(atom.load(std::memory_order::relaxed));

			return *this;
		}

		volatile MovableAtomicImplTrivial& operator=(const value_type& atom)
			volatile noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(atom.load(std::memory_order::relaxed));

			return *this;
		}

		volatile MovableAtomicImplTrivial& operator=(value_type&& atom)
			volatile noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(atom.load(std::memory_order::relaxed));

			return *this;
		}

		MovableAtomicImplTrivial& operator=(MovableAtomicImplTrivial&& other)
			noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(other.myValue.load(std::memory_order::relaxed));

			return *this;
		}

		MovableAtomicImplTrivial& operator=(volatile MovableAtomicImplTrivial&& other)
			noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(other.myValue.load(std::memory_order::relaxed));

			return *this;
		}

		volatile MovableAtomicImplTrivial& operator=(MovableAtomicImplTrivial&& other)
			volatile noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(other.myValue.load(std::memory_order::relaxed));

			return *this;
		}

		volatile MovableAtomicImplTrivial& operator=(volatile MovableAtomicImplTrivial&& other)
			volatile noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			myValue.store(other.myValue.load(std::memory_order::relaxed));

			return *this;
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
			noexcept(nothrow_addable_with<integral_type, U>)
		{
			return myValue.fetch_add(operand, std::forward<U>(order));
		}
		
		template<typename U> requires assignable_from<integral_type, U>
		integral_type FetchSubtract(U&& operand, const std::memory_order order = std::memory_order_seq_cst)
			noexcept(nothrow_subtractable_with<integral_type, U>)
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
		bool Equals(const MovableAtomicImplTrivial& other, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const noexcept(IsNothrowReadable)
		{
			return other.myValue.load(rhs_order) == myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const volatile MovableAtomicImplTrivial& other, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const noexcept(IsNothrowReadable)
		{
			return other.myValue.load(rhs_order) == myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const MovableAtomicImplTrivial& other, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const volatile noexcept(IsNothrowReadable)
		{
			return other.myValue.load(rhs_order) == myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const volatile MovableAtomicImplTrivial& other, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const volatile noexcept(IsNothrowReadable)
		{
			return other.myValue.load(rhs_order) == myValue.load(order);
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
		MovableAtomicImplTrivial(const MovableAtomicImplTrivial&) = delete;
		MovableAtomicImplTrivial& operator=(const MovableAtomicImplTrivial&) = delete;
	};
}

export namespace std
{
	template<typename T, typename U>
	T exchange(iconer::util::MovableAtomicImplTrivial<T>& lhs, U&& rhs, std::memory_order init_order = std::memory_order::acquire, std::memory_order final_order = std::memory_order::release)
		noexcept(is_nothrow_convertible_v<T, U&&>)
	{
		static_assert(is_convertible_v<T, U&&>);

		auto lval = lhs.Load(init_order);
		lhs.Store(std::forward<U>(rhs), final_order);
		return std::move(lval);
	}
}
