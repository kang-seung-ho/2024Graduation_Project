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

		constexpr ReadOnly() noexcept(nothrow_default_constructibles<T>)
			: myValue()
		{}

		template<typename U>
			requires (constructible_from<T, U>)
		constexpr ReadOnly(U&& value) noexcept(nothrow_constructible<T, U>)
			: myValue(std::forward<U>(value))
		{}

		template<typename... Args>
			requires (constructible_from<T, Args...>)
		constexpr ReadOnly(std::in_place_t, Args&&... args) noexcept(nothrow_constructible<T, Args...>)
			: myValue(std::forward<Args>(args)...)
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

		template<typename U>
		ReadOnly& operator=(U&&) = delete;
	};

	template<typename T>
	struct [[nodiscard]] ReadOnly<T*>
	{
		T* myValue;

		constexpr ReadOnly() noexcept
			: myValue()
		{}

		constexpr ReadOnly(T* ptr) noexcept
			: myValue(ptr)
		{}

		[[nodiscard]]
		constexpr T* operator->() noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr const T* operator->() const noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr operator T* () & noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr operator const T* () const& noexcept
		{
			return myValue;
		}

		[[nodiscard]]
		constexpr operator T* () && noexcept
		{
			return std::exchange(myValue, nullptr);
		}

		[[nodiscard]]
		constexpr operator const T* () const&& noexcept
		{
			return std::exchange(myValue, nullptr);
		}

		template<typename U>
		ReadOnly& operator=(U&&) = delete;
	};
}
