export module Iconer.Utility.ReadOnly;
import Iconer.Utility.TypeTraits;

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

		constexpr ReadOnly(const ReadOnly&) noexcept(nothrow_copy_constructibles<T>) = default;
		constexpr ReadOnly(ReadOnly&&) noexcept(nothrow_move_constructibles<T>) = default;

		ReadOnly& operator=(const ReadOnly&) noexcept(nothrow_copy_assignables<T>) = delete;
		ReadOnly& operator=(ReadOnly&&) noexcept(nothrow_move_assignables<T>) = delete;

		template<typename U>
		ReadOnly& operator=(U&&) = delete;
	};
}
