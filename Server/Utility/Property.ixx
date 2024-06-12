module;
export module Iconer.Utility.Property;
import Iconer.Utility.TypeTraits;
import Iconer.Utility.IProperty;

export namespace iconer::util
{
	template<typename T, typename Context = void> class Property;

	template<arithmetics T>
	class Property<T, void> final : private IProperty<T, void>
	{
	public:
		using super = IProperty<T, void>;
		using this_class = Property<T, void>;
		using value_type = super::value_type;

		template<typename U>
		using rebind = Property<U>;

		using super::super;

		constexpr this_class& operator=(const value_type& value)
		{
			super::SetValue(value);

			return *this;
		}

		constexpr this_class& operator=(value_type&& value)
		{
			super::SetValue(std::move(value));

			return *this;
		}

		template<typename U>
			requires assignable_from<value_type, const U&>
		constexpr this_class& operator=(const rebind<U>& other)
		{
			this->operator=(other.myValue);

			return *this;
		}

		template<typename U>
			requires assignable_from<value_type, U&&>
		constexpr this_class& operator=(rebind<U>&& other)
		{
			this->operator=(std::move(other.myValue));

			return *this;
		}

		template<typename U>
			requires assignable_from<value_type, U>
		constexpr this_class& operator=(U&& value)
		{
			this->operator=(std::forward<U>(value));

			return *this;
		}

		template<typename U>
			requires addable_with<value_type, U>
		constexpr Property& operator+=(U&& value)
		{
			super::SetValue(super::GetValue() + value);

			return *this;
		}

		template<typename U>
			requires addable_with<value_type, const U&>
		constexpr Property& operator+=(const rebind<U>& other)
		{
			super::SetValue(super::GetValue() + static_cast<const U&>(other));

			return *this;
		}

		template<typename U>
			requires addable_with<value_type, U&&>
		constexpr Property& operator+=(rebind<U>&& other)
		{
			super::SetValue(super::GetValue() + static_cast<U&&>(other));

			return *this;
		}

		template<typename U>
			requires subtractable_with<value_type, U>
		constexpr Property& operator-=(U&& value)
		{
			super::SetValue(super::GetValue() - value);

			return *this;
		}

		template<typename U>
			requires subtractable_with<value_type, const U&>
		constexpr Property& operator-=(const rebind<U>& other)
		{
			super::SetValue(super::GetValue() - static_cast<const U&>(other));

			return *this;
		}

		template<typename U>
			requires subtractable_with<value_type, U&&>
		constexpr Property& operator-=(rebind<U>&& other)
		{
			super::SetValue(super::GetValue() - static_cast<U&&>(other));

			return *this;
		}

		template<typename U>
			requires multipliable_with<value_type, U>
		constexpr Property& operator*=(U&& value)
		{
			super::SetValue(super::GetValue() * value);

			return *this;
		}

		template<typename U>
			requires multipliable_with<value_type, const U&>
		constexpr Property& operator*=(const rebind<U>& other)
		{
			super::SetValue(super::GetValue() * static_cast<const U&>(other));

			return *this;
		}

		template<typename U>
			requires multipliable_with<value_type, U&&>
		constexpr Property& operator*=(rebind<U>&& other)
		{
			super::SetValue(super::GetValue() * static_cast<U&&>(other));

			return *this;
		}

		template<typename U>
			requires dividable_with<value_type, U>
		constexpr Property& operator/=(U&& value)
		{
			super::SetValue(super::GetValue() / value);

			return *this;
		}

		template<typename U>
			requires dividable_with<value_type, const U&>
		constexpr Property& operator/=(const rebind<U>& other)
		{
			super::SetValue(super::GetValue() / static_cast<const U&>(other));

			return *this;
		}

		template<typename U>
			requires dividable_with<value_type, U&&>
		constexpr Property& operator/=(rebind<U>&& other)
		{
			super::SetValue(super::GetValue() / static_cast<U&&>(other));

			return *this;
		}

		[[nodiscard]]
		constexpr Property& operator++() noexcept
		{
			super::SetValue(super::GetValue() + 1);

			return *this;
		}

		[[nodiscard]]
		constexpr value_type operator++(int) noexcept
		{
			auto result = super::GetValue() + 1;
			super::SetValue(result);

			return result;
		}

		[[nodiscard]]
		constexpr Property& operator--() noexcept
		{
			super::SetValue(super::GetValue() - 1);

			return *this;
		}

		[[nodiscard]]
		constexpr value_type operator--(int) noexcept
		{
			auto result = super::GetValue() - 1;
			super::SetValue(result);

			return result;
		}

		[[nodiscard]]
		constexpr operator const value_type& () const&
		{
			return super::GetValue();
		}

		[[nodiscard]]
		constexpr operator value_type && ()&&
		{
			return std::move(super::GetValue());
		}

		Property(const Property&) noexcept = default;
		Property& operator=(const Property&) noexcept = default;
		Property(Property&&) noexcept = default;
		Property& operator=(Property&&) noexcept = default;
	};

	template<arithmetics T, classes Context>
	class Property<T, Context> final : private IProperty<T, Context>
	{
	public:
		using super = IProperty<T, Context>;
		using this_class = Property<T, Context>;
		using value_type = super::value_type;

		template<typename U>
		using rebind = Property<U>;

		using super::super;

		constexpr this_class& operator=(const value_type& value)
		{
			super::SetValue(value);

			return *this;
		}

		constexpr this_class& operator=(value_type&& value)
		{
			super::SetValue(std::move(value));

			return *this;
		}

		template<typename U>
			requires assignable_from<value_type, const U&>
		constexpr this_class& operator=(const rebind<U>& other)
		{
			this->operator=(other.myValue);

			return *this;
		}

		template<typename U>
			requires assignable_from<value_type, U&&>
		constexpr this_class& operator=(rebind<U>&& other)
		{
			this->operator=(std::move(other.myValue));

			return *this;
		}

		template<typename U>
			requires assignable_from<value_type, U>
		constexpr this_class& operator=(U&& value)
		{
			this->operator=(std::forward<U>(value));

			return *this;
		}

		template<typename U>
			requires addable_with<value_type, U>
		constexpr Property& operator+=(U&& value)
		{
			super::SetValue(super::GetValue() + value);

			return *this;
		}

		template<typename U>
			requires addable_with<value_type, const U&>
		constexpr Property& operator+=(const rebind<U>& other)
		{
			super::SetValue(super::GetValue() + static_cast<const U&>(other));

			return *this;
		}

		template<typename U>
			requires addable_with<value_type, U&&>
		constexpr Property& operator+=(rebind<U>&& other)
		{
			super::SetValue(super::GetValue() + static_cast<U&&>(other));

			return *this;
		}

		template<typename U>
			requires subtractable_with<value_type, U>
		constexpr Property& operator-=(U&& value)
		{
			super::SetValue(super::GetValue() - value);

			return *this;
		}

		template<typename U>
			requires subtractable_with<value_type, const U&>
		constexpr Property& operator-=(const rebind<U>& other)
		{
			super::SetValue(super::GetValue() - static_cast<const U&>(other));

			return *this;
		}

		template<typename U>
			requires subtractable_with<value_type, U&&>
		constexpr Property& operator-=(rebind<U>&& other)
		{
			super::SetValue(super::GetValue() - static_cast<U&&>(other));

			return *this;
		}

		template<typename U>
			requires multipliable_with<value_type, U>
		constexpr Property& operator*=(U&& value)
		{
			super::SetValue(super::GetValue() * value);

			return *this;
		}

		template<typename U>
			requires multipliable_with<value_type, const U&>
		constexpr Property& operator*=(const rebind<U>& other)
		{
			super::SetValue(super::GetValue() * static_cast<const U&>(other));

			return *this;
		}

		template<typename U>
			requires multipliable_with<value_type, U&&>
		constexpr Property& operator*=(rebind<U>&& other)
		{
			super::SetValue(super::GetValue() * static_cast<U&&>(other));

			return *this;
		}

		template<typename U>
			requires dividable_with<value_type, U>
		constexpr Property& operator/=(U&& value)
		{
			super::SetValue(super::GetValue() / value);

			return *this;
		}

		template<typename U>
			requires dividable_with<value_type, const U&>
		constexpr Property& operator/=(const rebind<U>& other)
		{
			super::SetValue(super::GetValue() / static_cast<const U&>(other));

			return *this;
		}

		template<typename U>
			requires dividable_with<value_type, U&&>
		constexpr Property& operator/=(rebind<U>&& other)
		{
			super::SetValue(super::GetValue() / static_cast<U&&>(other));

			return *this;
		}

		[[nodiscard]]
		constexpr Property& operator++() noexcept
		{
			super::SetValue(super::GetValue() + 1);

			return *this;
		}

		[[nodiscard]]
		constexpr value_type operator++(int) noexcept
		{
			auto result = super::GetValue() + 1;
			super::SetValue(result);

			return result;
		}

		[[nodiscard]]
		constexpr Property& operator--() noexcept
		{
			super::SetValue(super::GetValue() - 1);

			return *this;
		}

		[[nodiscard]]
		constexpr value_type operator--(int) noexcept
		{
			auto result = super::GetValue() - 1;
			super::SetValue(result);

			return result;
		}

		[[nodiscard]]
		constexpr operator const value_type& () const&
		{
			return super::GetValue();
		}

		[[nodiscard]]
		constexpr operator value_type && ()&&
		{
			return std::move(super::GetValue());
		}

		Property(const Property&) noexcept = default;
		Property& operator=(const Property&) noexcept = default;
		Property(Property&&) noexcept = default;
		Property& operator=(Property&&) noexcept = default;
	};

	template<>
	class Property<bool, void> final : private IProperty<bool, void>
	{
	public:
		using super = IProperty<bool, void>;
		using this_class = Property<bool, void>;
		using value_type = super::value_type;

		using super::super;

		constexpr this_class& operator=(const value_type& value)
		{
			super::SetValue(value);

			return *this;
		}

		constexpr this_class& operator=(value_type&& value)
		{
			super::SetValue(std::move(value));

			return *this;
		}

		constexpr this_class& operator|=(const value_type& value)
		{
			super::SetValue(super::GetValue() || value);

			return *this;
		}

		constexpr this_class& operator|=(value_type&& value)
		{
			super::SetValue(super::GetValue() || std::move(value));

			return *this;
		}

		constexpr this_class& operator&=(const value_type& value)
		{
			super::SetValue(super::GetValue() && value);

			return *this;
		}

		constexpr this_class& operator&=(value_type&& value)
		{
			super::SetValue(super::GetValue() && std::move(value));

			return *this;
		}

		[[nodiscard]]
		constexpr operator value_type() const
		{
			return super::GetValue();
		}

		Property(const Property&) noexcept = default;
		Property& operator=(const Property&) noexcept = default;
		Property(Property&&) noexcept = default;
		Property& operator=(Property&&) noexcept = default;
	};

	template<classes Context>
	class Property<bool, Context> final : private IProperty<bool, Context>
	{
	public:
		using super = IProperty<bool, Context>;
		using this_class = Property<bool, Context>;
		using value_type = super::value_type;

		using super::super;

		constexpr this_class& operator=(const value_type& value)
		{
			super::SetValue(value);

			return *this;
		}

		constexpr this_class& operator=(value_type&& value)
		{
			super::SetValue(std::move(value));

			return *this;
		}

		constexpr this_class& operator|=(const value_type& value)
		{
			super::SetValue(super::GetValue() || value);

			return *this;
		}

		constexpr this_class& operator|=(value_type&& value)
		{
			super::SetValue(super::GetValue() || std::move(value));

			return *this;
		}

		constexpr this_class& operator&=(const value_type& value)
		{
			super::SetValue(super::GetValue() && value);

			return *this;
		}

		constexpr this_class& operator&=(value_type&& value)
		{
			super::SetValue(super::GetValue() && std::move(value));

			return *this;
		}

		[[nodiscard]]
		constexpr operator value_type() const
		{
			return super::GetValue();
		}

		Property(const Property&) noexcept = default;
		Property& operator=(const Property&) noexcept = default;
		Property(Property&&) noexcept = default;
		Property& operator=(Property&&) noexcept = default;
	};

	template<typename T>
	Property(T) -> Property<T, void>;

	template<typename T>
	Property(function_t<void, T>) -> Property<clean_t<T>, void>;

	template<typename T>
	Property(nothrow_function_t<void, T>) -> Property<clean_t<T>, void>;

	template<typename T>
	Property(T, function_t<void, T>) -> Property<T, void>;

	template<typename T>
	Property(T, nothrow_function_t<void, T>) -> Property<T, void>;

	template<typename T, invocable<T> Delegate>
	Property(T, Delegate) -> Property<T, void>;


	template<classes Context, typename T>
	Property(Context*, method_t<Context, void, T>) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(Context*, const_method_t<Context, void, T>) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(const Context*, const_method_t<Context, void, T>) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(Context*, method_t<Context, void, T>, T) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(Context*, const_method_t<Context, void, T>, T) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(const Context*, const_method_t<Context, void, T>, T) -> Property<T, Context>;


	template<classes Context, typename T>
	Property(Context*, method_t<Context, void, std::add_lvalue_reference_t<T>>) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(Context*, const_method_t<Context, void, std::add_lvalue_reference_t<T>>) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(const Context*, const_method_t<Context, void, std::add_lvalue_reference_t<T>>) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(Context*, method_t<Context, void, std::add_lvalue_reference_t<T>>, T) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(Context*, const_method_t<Context, void, std::add_lvalue_reference_t<T>>, T) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(const Context*, const_method_t<Context, void, std::add_lvalue_reference_t<T>>, T) -> Property<T, Context>;


	template<classes Context, typename T>
	Property(Context*, method_t<Context, void, std::add_lvalue_reference_t<std::add_const_t<T>>>) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(Context*, const_method_t<Context, void, std::add_lvalue_reference_t<std::add_const_t<T>>>) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(const Context*, const_method_t<Context, void, std::add_lvalue_reference_t<std::add_const_t<T>>>) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(Context*, method_t<Context, void, std::add_lvalue_reference_t<std::add_const_t<T>>>, T) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(Context*, const_method_t<Context, void, std::add_lvalue_reference_t<std::add_const_t<T>>>, T) -> Property<T, Context>;

	template<classes Context, typename T>
	Property(const Context*, const_method_t<Context, void, std::add_lvalue_reference_t<std::add_const_t<T>>>, T) -> Property<T, Context>;
}

module:private;
