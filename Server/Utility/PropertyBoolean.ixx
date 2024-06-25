module;
#include <type_traits>

export module Iconer.Utility.Property:Boolean;
import Iconer.Utility.TypeTraits;
import Iconer.Utility.IProperty;
export import :Definitions;

export namespace iconer::util
{
	template<>
	class Property<bool, void> final : private iconer::util::detail::IProperty<bool, void>
	{
	public:
		using super = iconer::util::detail::IProperty<bool, void>;
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

		template<integrals U>
		constexpr void operator=(U&& value) = delete;
		template<floats U>
		constexpr void operator=(U&& value) = delete;

		Property(const Property&) noexcept = default;
		Property& operator=(const Property&) noexcept = default;
		Property(Property&&) noexcept = default;
		Property& operator=(Property&&) noexcept = default;
	};

	template<typename Context>
	class Property<bool, Context> final : private iconer::util::detail::IProperty<bool, Context>
	{
	public:
		using super = iconer::util::detail::IProperty<bool, Context>;
		using this_class = Property<bool, Context>;
		using value_type = super::value_type;

		friend Context;

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

		template<integrals U>
		constexpr void operator=(U&& value) = delete;
		template<floats U>
		constexpr void operator=(U&& value) = delete;

		Property(const Property&) noexcept = default;
		Property& operator=(const Property&) noexcept = default;
		Property(Property&&) noexcept = default;
		Property& operator=(Property&&) noexcept = default;
	};
}