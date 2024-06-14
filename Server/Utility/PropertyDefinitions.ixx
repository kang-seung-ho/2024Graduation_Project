export module Iconer.Utility.Property:Definitions;
import Iconer.Utility.TypeTraits;
import Iconer.Utility.IProperty;

export namespace iconer::util
{
	template<typename T, typename Context = void>
	class Property final : private iconer::util::detail::IProperty<T, Context>
	{
	public:
		using super = iconer::util::detail::IProperty<T, Context>;
		using this_class = Property<T, Context>;
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

		template<typename U, typename C>
		constexpr this_class& operator=(const Property<U, C>& other)
		{
			super::SetValue(other);

			return *this;
		}

		template<typename U, typename C>
		constexpr this_class& operator=(Property<U, C>&& other)
		{
			super::SetValue(std::move(other));

			return *this;
		}

		[[nodiscard]]
		constexpr operator value_type& ()&
		{
			return super::GetValue();
		}

		[[nodiscard]]
		constexpr operator value_type const& () const&
		{
			return super::GetValue();
		}

		[[nodiscard]]
		constexpr operator value_type && ()&&
		{
			return std::move(super::GetValue());
		}

		[[nodiscard]]
		constexpr operator value_type const&& () const&&
		{
			return std::move(super::GetValue());
		}

		Property(const Property&) noexcept = default;
		Property& operator=(const Property&) noexcept = default;
		Property(Property&&) noexcept = default;
		Property& operator=(Property&&) noexcept = default;
	};

	template<typename T>
	class Property<T, void> final : private iconer::util::detail::IProperty<T, void>
	{
	public:
		using super = iconer::util::detail::IProperty<T, void>;
		using this_class = Property<T, void>;
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

		[[nodiscard]]
		constexpr operator value_type& ()&
		{
			return super::GetValue();
		}

		[[nodiscard]]
		constexpr operator value_type const& () const&
		{
			return super::GetValue();
		}

		[[nodiscard]]
		constexpr operator value_type && ()&&
		{
			return std::move(super::GetValue());
		}

		[[nodiscard]]
		constexpr operator value_type const&& () const&&
		{
			return std::move(super::GetValue());
		}

		Property(const Property&) noexcept = default;
		Property& operator=(const Property&) noexcept = default;
		Property(Property&&) noexcept = default;
		Property& operator=(Property&&) noexcept = default;
	};

	template<typename L, typename R, typename CL, typename CR>
		requires std::equality_comparable_with<L, R>
	[[nodiscard]]
	constexpr bool operator==(const Property<L, CL>& lhs, const Property<R, CR>& rhs)
		noexcept(noexcept(std::declval<const L&>() == std::declval<const R&>()))
	{
		return static_cast<const L&>(lhs) == static_cast<const R&>(rhs);
	}

	template<typename L, typename R, typename CL, typename CR>
		requires std::totally_ordered_with<L, R>
	[[nodiscard]]
	constexpr bool operator==(const Property<L, CL>& lhs, const Property<R, CR>& rhs)
		noexcept(noexcept(std::declval<const L&>() == std::declval<const R&>()))
	{
		return static_cast<const L&>(lhs) == static_cast<const R&>(rhs);
	}

	template<typename L, typename R, typename CL, typename CR>
		requires std::totally_ordered_with<L, R>
	[[nodiscard]]
	constexpr auto operator<=>(const Property<L, CL>& lhs, const Property<R, CR>& rhs)
		noexcept(noexcept(std::declval<const L&>() <=> std::declval<const R&>()))
	{
		return static_cast<const L&>(lhs) <=> static_cast<const R&>(rhs);
	}

	template<typename L, typename CL, typename U>
		requires std::totally_ordered_with<L, U>
	[[nodiscard]]
	constexpr auto operator<=>(const Property<L, CL>& lhs, const U& rhs)
		noexcept(noexcept(std::declval<const L&>() <=> std::declval<const U&>()))
		-> decltype(std::declval<const L&>() <=> std::declval<const U&>())
	{
		return static_cast<const L&>(lhs) <=> rhs;
	}
}
