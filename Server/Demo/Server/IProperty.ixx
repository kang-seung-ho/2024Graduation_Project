export module Iconer.Utility.Property:IProperty;
import Iconer.Utility.Constraints;
import <concepts>;
import <utility>;

export namespace iconer::util
{
	template<notvoids T, typename Predicate>
	class IProperty;
}

export namespace std
{
	template<typename LT, typename RT, typename P>
	constexpr void swap(iconer::util::IProperty<LT, P>& lhs, iconer::util::IProperty<RT, P>& rhs) noexcept(iconer::nothrow_swappables<LT, RT, P> and is_nothrow_swappable_with_v<LT, RT>);
}

export namespace iconer::util
{
	template<notvoids T, typename Predicate>
	class IProperty final
	{
	public:
		static_assert(not is_specialization_v<T, IProperty>);

		using self_type = IProperty<T, Predicate>;
		using value_type = T;
		using functor_type = Predicate;
		using reference = add_lvalue_reference_t<T>;
		using const_reference = add_lvalue_reference_t<add_const_t<T>>;
		using pointer = add_pointer_t<T>;
		using const_pointer = add_pointer_t<add_const_t<T>>;

		template<typename U>
		using rebind_t = IProperty<U, functor_type>;

		static inline constexpr bool IsActualNothrow = nothrow_invocables<functor_type, T>;
		static inline constexpr bool IsCopyable = copyable<T> and copyable<functor_type>;
		static inline constexpr bool IsMovable = movable<T> and movable<functor_type>;

		constexpr IProperty()
			noexcept(nothrow_default_constructibles<T, functor_type>)
			requires default_initializables<T, functor_type> = default;
		constexpr ~IProperty()
			noexcept(nothrow_destructibles<T, functor_type>) = default;

		explicit constexpr IProperty(const self_type& other)
			noexcept(nothrow_copy_constructibles<T, functor_type>) requires IsCopyable
			: myValue(other.myValue)
			, myDelegate(other.myDelegate)
		{
		}

		explicit constexpr IProperty(self_type&& other)
			noexcept(nothrow_move_constructibles<T, functor_type>) requires IsMovable
			: myValue(std::move(other.myValue))
			, myDelegate(std::move(other.myDelegate))
		{
		}

		template<typename U, invocables<T> Fn>
		explicit constexpr IProperty(U&& init_value, Fn&& delegate)
			noexcept(nothrow_constructible<T, U&&> and nothrow_constructible<functor_type, Fn&&>)
			: myValue(std::forward<U>(init_value))
			, myDelegate(std::forward<Fn>(delegate))
		{
		}

		template<typename U>
		constexpr IProperty& operator=(const rebind_t<U>& other)
			noexcept(nothrow_assignable<T, const U&> and IsActualNothrow)
		{
			myValue = other.myValue;
			myDelegate(myValue);

			return *this;
		}

		template<typename U>
		constexpr IProperty& operator=(rebind_t<U>&& other)
			noexcept(nothrow_assignable<T, U&&> and IsActualNothrow)
		{
			myValue = std::move(other.myValue);
			myDelegate(myValue);

			return *this;
		}

		template<typename U>
		constexpr IProperty& operator=(U&& value)
			noexcept(nothrow_assignable<T, U&&> and IsActualNothrow)
		{
			myValue = std::forward<U>(value);
			myDelegate(myValue);

			return *this;
		}

		constexpr pointer operator->() noexcept
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return myValue;
			}
			else
			{
				return std::addressof(myValue);
			}
		}

		constexpr const_pointer operator->() const noexcept
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return myValue;
			}
			else
			{
				return std::addressof(myValue);
			}
		}

		[[nodiscard]]
		constexpr operator T& () & noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr operator const T& () const& noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr operator T && () && noexcept
		{
			return std::move(myValue);
		}

		[[nodiscard]]
		constexpr operator const T && () const&& noexcept
		{
			return std::move(myValue);
		}

		template<typename U, typename V>
		[[nodiscard]]
		friend constexpr bool operator==(const rebind_t<U>& lhs, const rebind_t<U>& rhs) noexcept
		{
			return lhs.myValue == rhs.myValue;
		}

		template<typename U, typename V>
		[[nodiscard]]
		friend constexpr std::strong_ordering operator<=>(const rebind_t<U>& lhs, const rebind_t<U>& rhs) noexcept
		{
			return lhs.myValue <=> rhs.myValue;
		}

		template<typename U>
			requires (not is_specialization_v<U, IProperty>)
		[[nodiscard]]
		friend constexpr std::strong_ordering operator<=>(const IProperty& lhs, U&& value) noexcept
		{
			return lhs.myValue <=> value;
		}

		template<typename LT, typename RT, typename P>
		friend void std::swap(iconer::util::IProperty<LT, P>& lhs, iconer::util::IProperty<RT, P>& rhs);

		constexpr IProperty(const IProperty&) noexcept(nothrow_copy_constructibles<value_type, functor_type>) = default;
		constexpr IProperty& operator=(const IProperty&) noexcept(nothrow_copy_assignables<value_type, functor_type>) = default;
		constexpr IProperty(IProperty&&) noexcept(nothrow_copy_constructibles<value_type, functor_type>) = default;
		constexpr IProperty& operator=(IProperty&&) noexcept(nothrow_move_assignables<value_type, functor_type>) = default;

	protected:
		value_type myValue;
		functor_type myDelegate;
	};

	template<notvoids T>
	class IProperty<T, void> final
	{
	public:
		static_assert(not is_specialization_v<T, IProperty>);

		using self_type = IProperty<T, void>;
		using value_type = T;
		using functor_type = void;
		using reference = add_lvalue_reference_t<T>;
		using const_reference = add_lvalue_reference_t<add_const_t<T>>;
		using pointer = add_pointer_t<T>;
		using const_pointer = add_pointer_t<add_const_t<T>>;

		template<typename U>
		using rebind_t = IProperty<U, void>;

		static inline constexpr bool IsCopyable = copyable<T>;
		static inline constexpr bool IsMovable = movable<T>;

		constexpr IProperty()
			noexcept(nothrow_default_constructibles<T>)
			requires default_initializables<T> = default;
		constexpr ~IProperty()
			noexcept(nothrow_destructibles<T>) = default;

		template<typename Visitor = CopyToForward, typename ValueProjection = CopyToForward>
		explicit constexpr IProperty(const self_type& other, Visitor&& visitor = {}, ValueProjection&& proj = {})
			noexcept(nothrow_copy_constructibles<T>) requires IsCopyable
			: myValue(std::invoke(proj, other.myValue))
		{
		}

		template<typename Visitor = MoveToForward, typename ValueProjection = MoveToForward>
		explicit constexpr IProperty(self_type&& other, Visitor&& visitor = {}, ValueProjection&& proj = {})
			noexcept(nothrow_move_constructibles<T>) requires IsMovable
			: myValue(std::invoke(proj, other.myValue))
		{
		}

		template<typename U>
		explicit constexpr IProperty(U&& init_value)
			noexcept(nothrow_constructible<T, U&&>)
			: myValue(std::forward<U>(init_value))
		{
		}

		template<typename U>
		constexpr IProperty& operator=(const rebind_t<U>& other)
			noexcept(nothrow_assignable<T, const U&>)
		{
			myValue = other.myValue;

			return *this;
		}

		template<typename U>
		constexpr IProperty& operator=(rebind_t<U>&& other)
			noexcept(nothrow_assignable<T, U&&>)
		{
			myValue = std::move(other.myValue);

			return *this;
		}

		template<typename U>
		constexpr IProperty& operator=(U&& value)
			noexcept(nothrow_assignable<T, U&&>)
		{
			myValue = std::forward<U>(value);

			return *this;
		}

		constexpr pointer operator->() noexcept
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return myValue;
			}
			else
			{
				return std::addressof(myValue);
			}
		}

		constexpr const_pointer operator->() const noexcept
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return myValue;
			}
			else
			{
				return std::addressof(myValue);
			}
		}

		[[nodiscard]]
		constexpr operator T& () & noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr operator const T& () const& noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr operator T && () && noexcept
		{
			return std::move(myValue);
		}

		[[nodiscard]]
		constexpr operator const T && () const&& noexcept
		{
			return std::move(myValue);
		}

		template<typename U, typename V>
		[[nodiscard]]
		friend constexpr bool operator==(const rebind_t<U>& lhs, const rebind_t<U>& rhs) noexcept
		{
			return lhs.myValue == rhs.myValue;
		}

		template<typename U, typename V>
		[[nodiscard]]
		friend constexpr std::strong_ordering operator<=>(const rebind_t<U>& lhs, const rebind_t<U>& rhs) noexcept
		{
			return lhs.myValue <=> rhs.myValue;
		}

		template<typename U>
			requires (not is_specialization_v<U, IProperty>)
		[[nodiscard]]
		friend constexpr std::strong_ordering operator<=>(const IProperty& lhs, U&& value) noexcept
		{
			return lhs.myValue <=> value;
		}

		template<typename LT, typename RT, typename P>
		friend void std::swap(iconer::util::IProperty<LT, P>& lhs, iconer::util::IProperty<RT, P>& rhs);

		constexpr IProperty(const IProperty&) noexcept(nothrow_copy_constructibles<value_type>) = default;
		constexpr IProperty& operator=(const IProperty&) noexcept(nothrow_copy_assignables<value_type>) = default;
		constexpr IProperty(IProperty&&) noexcept(nothrow_copy_constructibles<value_type>) = default;
		constexpr IProperty& operator=(IProperty&&) noexcept(nothrow_move_assignables<value_type>) = default;

	protected:
		value_type myValue;
	};
}

export namespace std
{
	template<typename LT, typename RT, typename P>
	constexpr
		void
		swap(iconer::util::IProperty<LT, P>& lhs, iconer::util::IProperty<RT, P>& rhs)
		noexcept(iconer::nothrow_swappables<LT, RT, P> and is_nothrow_swappable_with_v<LT, RT>)
	{
		static_assert(swappable<LT>);
		static_assert(swappable<RT>);
		static_assert(swappable_with<LT, RT>);
		static_assert(swappable<P>);

		using ::std::swap;

		swap(lhs.myValue, rhs.myValue);

		if constexpr (iconer::notvoids<P>)
		{
			swap(lhs.myDelegate, rhs.myDelegate);
		}
	}
}
