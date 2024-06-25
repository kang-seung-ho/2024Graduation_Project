export module Iconer.Utility.ReadOnly;
import Iconer.Utility.TypeTraits;
import <type_traits>;
import <utility>;

export namespace iconer::util
{
	template<typename T>
	struct [[nodiscard]] ReadOnly
	{
		T myValue;

		[[nodiscard]]
		constexpr T* operator->() noexcept
		{
			return std::addressof(myValue);
		}

		[[nodiscard]]
		constexpr const T* operator->() const noexcept
		{
			return std::addressof(myValue);
		}

		[[nodiscard]]
		constexpr operator T& () & noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr operator T const& () const& noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr operator T && () && noexcept
		{
			return std::move(myValue);
		}

		[[nodiscard]]
		constexpr operator T const&& () const&& noexcept
		{
			return std::move(myValue);
		}

		template<typename U>
		ReadOnly& operator=(U&&) = delete;
	};
}
