export module Iconer.Utility.MovableAtomic:NonTrivial;
import Iconer.Utility.Constraints;
import Iconer.Utility.Atomic;
import :Storage;
import <compare>;
import <bit>;
import <utility>;
import <memory>;

namespace iconer::util
{
	template<typename T>
	class MovableAtomicImplNonTrivial : public MovableAtomicStorage<std::atomic<T*>>
	{
	public:
		using Super = MovableAtomicStorage<std::atomic<T*>>;
		using value_type = Super::value_type;
		using pointer = Super::integral_type;
		using integral_atomic = std::atomic<T>;
		using integral_type = T;

		static inline constexpr bool IsCopyable = copyable<integral_type>;
		static inline constexpr bool IsMovable = movable<integral_type>;
		static inline constexpr bool IsNothrowReadable = nothrow_atomic_readable<value_type>;
		static inline constexpr bool IsNothrowWritable = nothrow_atomic_writable<value_type>;

	private:
		template<typename... Args>
		[[nodiscard]]
		static constexpr pointer Allocate(Args&&... args)
		{
			std::allocator<integral_type> alloc{};
			auto handle = alloc.allocate(1);
			return std::construct_at(handle, std::forward<Args>(args)...);
		}

		static constexpr void Dellocate(const pointer handle)
		{
			std::destroy_at(handle);
		}

	public:
		explicit MovableAtomicImplNonTrivial()
			: myValue()
		{
			std::allocator<integral_type> alloc{};
			auto handle = alloc.allocate(1);

			myValue.store(std::construct_at(handle), std::memory_order::relaxed);
		}

		template<typename... Args>
		explicit MovableAtomicImplNonTrivial(in_place_t, Args&&... args)
			: myValue()
		{
			auto handle = Allocate();

			myValue.store(std::construct_at(handle, std::forward<Args>(args)...), std::memory_order::relaxed);
		}

		~MovableAtomicImplNonTrivial()
			noexcept(nothrow_destructibles<value_type> and noexcept(std::destroy_at(std::declval<pointer>())))
		{
			std::destroy_at(std::atomic_exchange(std::addressof(myValue), nullptr));
		}

		explicit
			MovableAtomicImplNonTrivial(const integral_type& value) requires IsCopyable
			: MovableAtomicImplNonTrivial()
		{
			Store(value, std::memory_order::relaxed);
		}

		explicit constexpr
			MovableAtomicImplNonTrivial(integral_type&& value) requires IsMovable
			: MovableAtomicImplNonTrivial()
		{
			Store(static_cast<integral_type&&>(value), std::memory_order::relaxed);
		}

		explicit
			MovableAtomicImplNonTrivial(const integral_atomic& atom, std::memory_order order = std::memory_order::relaxed)
			: MovableAtomicImplNonTrivial()
		{
			Store(atom.load(order), std::memory_order::relaxed);
		}

		explicit
			MovableAtomicImplNonTrivial(integral_atomic&& atom, std::memory_order order = std::memory_order::relaxed)
			: MovableAtomicImplNonTrivial()
		{
			Store(atom.load(order), std::memory_order::relaxed);
		}

		explicit
			MovableAtomicImplNonTrivial(MovableAtomicImplNonTrivial&& other, std::memory_order order = std::memory_order::relaxed)
			: MovableAtomicImplNonTrivial()
		{
			Store(other.Load(order), std::memory_order::relaxed);
		}

		explicit
			MovableAtomicImplNonTrivial(volatile MovableAtomicImplNonTrivial&& other, std::memory_order order = std::memory_order::relaxed)
			: MovableAtomicImplNonTrivial()
		{
			Store(other.Load(order), std::memory_order::relaxed);
		}

		MovableAtomicImplNonTrivial& operator=(const integral_type& value)
			noexcept(IsNothrowWritable)
		{
			Store(value, std::memory_order::acq_rel);

			return *this;
		}

		MovableAtomicImplNonTrivial& operator=(integral_type&& value)
			noexcept(IsNothrowWritable)
		{
			Store(static_cast<integral_type&&>(value), std::memory_order::acq_rel);

			return *this;
		}

		MovableAtomicImplNonTrivial& operator=(const integral_atomic& atom)
			noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			Store(atom.load(std::memory_order::acq_rel), std::memory_order::acq_rel);

			return *this;
		}

		MovableAtomicImplNonTrivial& operator=(integral_atomic&& atom)
			noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			Store(atom.load(std::memory_order::acq_rel), std::memory_order::acq_rel);

			return *this;
		}

		volatile MovableAtomicImplNonTrivial& operator=(const integral_atomic& atom)
			volatile noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			Store(atom.load(std::memory_order::acq_rel), std::memory_order::acq_rel);

			return *this;
		}

		volatile MovableAtomicImplNonTrivial& operator=(integral_atomic&& atom)
			volatile noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			Store(atom.load(std::memory_order::acq_rel), std::memory_order::acq_rel);

			return *this;
		}

		MovableAtomicImplNonTrivial& operator=(MovableAtomicImplNonTrivial&& other)
			noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			Store(other.myValue.load(std::memory_order::acq_rel), std::memory_order::acq_rel);

			return *this;
		}

		MovableAtomicImplNonTrivial& operator=(volatile MovableAtomicImplNonTrivial&& other)
			noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			Store(other.myValue.load(std::memory_order::acq_rel), std::memory_order::acq_rel);

			return *this;
		}

		volatile MovableAtomicImplNonTrivial& operator=(MovableAtomicImplNonTrivial&& other)
			volatile noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			Store(other.myValue.load(std::memory_order::acq_rel), std::memory_order::acq_rel);

			return *this;
		}

		volatile MovableAtomicImplNonTrivial& operator=(volatile MovableAtomicImplNonTrivial&& other)
			volatile noexcept(IsNothrowReadable and IsNothrowWritable)
		{
			Store(other.myValue.load(std::memory_order::acq_rel), std::memory_order::acq_rel);

			return *this;
		}

		template<typename U>
		void Store(U&& value, std::memory_order order = std::memory_order::acq_rel)
			noexcept(IsNothrowWritable)
		{
			Exchange(std::forward<U>(value), order);
		}

		template<typename U>
		void Store(U&& value, std::memory_order order = std::memory_order::acq_rel)
			volatile noexcept(IsNothrowWritable)
		{
			Exchange(std::forward<U>(value), order);
		}

		[[nodiscard]]
		integral_type Load(std::memory_order order = std::memory_order::acq_rel)
		{
			return *myValue.load(order);
		}

		[[nodiscard]]
		integral_type Load(std::memory_order order = std::memory_order::acq_rel) volatile
		{
			return *myValue.load(order);
		}

		template<typename U>
		integral_type Exchange(U&& value, std::memory_order order = std::memory_order::acq_rel)
			noexcept
		{
			pointer newer = Allocate(std::forward<U>(value));
			auto old = std::atomic_exchange_explicit(std::addressof(myValue), newer, order);
			auto result = *old;

			Dellocate(old);
			return std::move(result);
		}

		template<typename U>
		integral_type Exchange(U&& value, std::memory_order order = std::memory_order::acq_rel)
			volatile noexcept
		{
			pointer newer = Allocate(std::forward<U>(value));
			auto old = std::atomic_exchange_explicit(std::addressof(myValue), newer, order);
			auto result = *old;

			Dellocate(old);
			return std::move(result);
		}

		[[nodiscard]]
		bool Equals(const integral_type& value, std::memory_order order = std::memory_order::relaxed)
			const noexcept(IsNothrowReadable)
		{
			return value == *myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const integral_type& value, std::memory_order order = std::memory_order::relaxed)
			const volatile
		{
			return value == *myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const integral_atomic& atom, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const
		{
			return atom.load(rhs_order) == *myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const volatile integral_atomic& atom, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const
		{
			return atom.load(rhs_order) == *myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const integral_atomic& atom, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const volatile
		{
			return atom.load(rhs_order) == *myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const volatile integral_atomic& atom, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const volatile
		{
			return atom.load(rhs_order) == *myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const MovableAtomicImplNonTrivial& other, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const
		{
			return *other.myValue.load(rhs_order) == *myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const volatile MovableAtomicImplNonTrivial& other, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const
		{
			return *other.myValue.load(rhs_order) == *myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const MovableAtomicImplNonTrivial& other, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const volatile
		{
			return *other.myValue.load(rhs_order) == *myValue.load(order);
		}

		[[nodiscard]]
		bool Equals(const volatile MovableAtomicImplNonTrivial& other, std::memory_order rhs_order = std::memory_order::relaxed, std::memory_order order = std::memory_order::relaxed)
			const volatile
		{
			return *other.myValue.load(rhs_order) == *myValue.load(order);
		}

		[[nodiscard]]
		explicit operator integral_type& () & noexcept
		{
			return *myValue.load();
		}

		[[nodiscard]]
		explicit operator const integral_type& () const& noexcept
		{
			return *myValue.load();
		}

		[[nodiscard]]
		explicit operator integral_type && () && noexcept
		{
			return std::move(*myValue.load());
		}

		[[nodiscard]]
		explicit operator const integral_type && () const&& noexcept
		{
			return std::move(*myValue.load());
		}

		[[nodiscard]]
		pointer operator->() const noexcept
		{
			return myValue.load();
		}

		[[nodiscard]]
		pointer operator->() const volatile noexcept
		{
			return myValue.load();
		}

		[[nodiscard]]
		bool operator==(const integral_type& value)
			const noexcept(IsNothrowReadable)
		{
			return Equals(value);
		}

		[[nodiscard]]
		auto operator<=>(const integral_type& value)
			const noexcept(IsNothrowReadable)
		{
			return value <=> *myValue.load(std::memory_order::relaxed);
		}

		[[nodiscard]]
		bool operator==(const integral_type& value)
			const volatile noexcept(IsNothrowReadable)
		{
			return Equals(value);
		}

		[[nodiscard]]
		auto operator<=>(const integral_type& value)
			const volatile noexcept(IsNothrowReadable)
		{
			return value <=> *myValue.load(std::memory_order::relaxed);
		}

		[[nodiscard]]
		bool operator==(const integral_atomic& atom)
			const noexcept(IsNothrowReadable)
		{
			return Equals(atom);
		}

		[[nodiscard]]
		auto operator<=>(const integral_atomic& atom)
			const noexcept(IsNothrowReadable)
		{
			return atom.load(std::memory_order::relaxed) <=> *myValue.load(std::memory_order::relaxed);
		}

		[[nodiscard]]
		bool operator==(const integral_atomic& atom)
			const volatile noexcept(IsNothrowReadable)
		{
			return Equals(atom);
		}

		[[nodiscard]]
		auto operator<=>(const integral_atomic& atom)
			const volatile noexcept(IsNothrowReadable)
		{
			return atom.load(std::memory_order::relaxed) <=> *myValue.load(std::memory_order::relaxed);
		}

		[[nodiscard]]
		bool operator==(const MovableAtomicImplNonTrivial& other)
			const noexcept(IsNothrowReadable)
		{
			return Equals(other);
		}

		[[nodiscard]]
		auto operator<=>(const MovableAtomicImplNonTrivial& other)
			const noexcept(IsNothrowReadable)
		{
			return other.myValue.load(std::memory_order::relaxed) <=> myValue.load(std::memory_order::relaxed);
		}

		[[nodiscard]]
		bool operator==(const volatile MovableAtomicImplNonTrivial& other)
			const noexcept(IsNothrowReadable)
		{
			return Equals(other);
		}

		[[nodiscard]]
		auto operator<=>(const volatile MovableAtomicImplNonTrivial& other)
			const noexcept(IsNothrowReadable)
		{
			return other.myValue.load(std::memory_order::relaxed) <=> myValue.load(std::memory_order::relaxed);
		}

		[[nodiscard]]
		bool operator==(const MovableAtomicImplNonTrivial& other)
			const volatile noexcept(IsNothrowReadable)
		{
			return Equals(other);
		}

		[[nodiscard]]
		auto operator<=>(const MovableAtomicImplNonTrivial& other)
			const volatile noexcept(IsNothrowReadable)
		{
			return other.myValue.load(std::memory_order::relaxed) <=> myValue.load(std::memory_order::relaxed);
		}

		[[nodiscard]]
		bool operator==(const volatile MovableAtomicImplNonTrivial& other)
			const volatile noexcept(IsNothrowReadable)
		{
			return Equals(other);
		}

		[[nodiscard]]
		auto operator<=>(const volatile MovableAtomicImplNonTrivial& other)
			const volatile noexcept(IsNothrowReadable)
		{
			return other.myValue.load(std::memory_order::relaxed) <=> myValue.load(std::memory_order::relaxed);
		}

	protected:
		value_type myValue;
	};
}

export namespace std
{
	template<typename T, typename U>
	T exchange(iconer::util::MovableAtomicImplNonTrivial<T>& lhs, U&& rhs, std::memory_order init_order = std::memory_order::acquire, std::memory_order final_order = std::memory_order::release)
		noexcept(is_nothrow_convertible_v<T, U&&>)
	{
		static_assert(is_convertible_v<T, U&&>);

		auto lval = lhs.Load(init_order);
		lhs.Store(std::forward<U>(rhs), final_order);
		return std::move(lval);
	}
}
