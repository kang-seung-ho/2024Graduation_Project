#pragma once
#include <type_traits>
#include <utility>

import Iconer.Utility.TypeTraits;
import Iconer.Utility.IProperty;

export namespace iconer::util
{
	template<>
	class Property<bool, void> final : private IProperty<bool, void>
	{
	public:
		using super = IProperty<bool, void>;
		using this_class = Property<bool>;
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
		using this_class = Property<bool>;
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
}
