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

		constexpr ReadOnly() noexcept(nothrow_default_constructibles<T>) = default;
		constexpr ~ReadOnly() noexcept(nothrow_destructibles<T>) = default;

		template<typename U>
		constexpr ReadOnly(U&& value) noexcept(nothrow_constructible<T, U>)
			: myValue(std::forward<U>(value))
		{}

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

		constexpr ReadOnly(const ReadOnly&) noexcept(nothrow_copy_constructibles<T>) = default;
		constexpr ReadOnly(ReadOnly&&) noexcept(nothrow_move_constructibles<T>) = default;
		constexpr ReadOnly& operator=(const ReadOnly&) noexcept(nothrow_copy_assignables<T>) = default;
		constexpr ReadOnly& operator=(ReadOnly&&) noexcept(nothrow_move_assignables<T>) = default;

		template<typename U>
		ReadOnly& operator=(U&&) = delete;
	};
}
