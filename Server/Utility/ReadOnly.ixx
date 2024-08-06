module;
#include <type_traits>
#include <utility>

export module Iconer.Utility.ReadOnly;
export import <new>;

export namespace iconer::util
{
	template<typename T>
	struct [[nodiscard]] ReadOnly
	{
		alignas(std::hardware_constructive_interference_size) T myValue;

		constexpr ReadOnly() noexcept(std::is_nothrow_default_constructible_v<T>)
			: myValue()
		{}

		template<typename U>
			requires (std::constructible_from<T, U>)
		constexpr ReadOnly(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
			: myValue(std::forward<U>(value))
		{}

		template<typename... Args>
			requires (std::constructible_from<T, Args...>)
		constexpr ReadOnly(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
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

	template<typename X, typename Y>
	constexpr bool operator==(const ReadOnly<X>& lhs, const ReadOnly<Y>& rhs) noexcept
	{
		return lhs.myValue == rhs.myValue;
	}

	template<typename X>
	constexpr bool operator==(const ReadOnly<X>& lhs, const X& rhs) noexcept
	{
		return lhs.myValue == rhs;
	}

	template<typename X, typename Y>
	constexpr bool operator==(const ReadOnly<X*>& lhs, const ReadOnly<Y*>& rhs) noexcept
	{
		return lhs.myValue == rhs.myValue;
	}

	template<typename X, typename Y>
	constexpr bool operator==(const ReadOnly<X*>& lhs, nullptr_t) noexcept
	{
		return lhs.myValue == nullptr;
	}
	template<typename X, typename Y>
	constexpr bool operator!=(const ReadOnly<X>& lhs, const ReadOnly<Y>& rhs) noexcept
	{
		return lhs.myValue != rhs.myValue;
	}

	template<typename X>
	constexpr bool operator!=(const ReadOnly<X>& lhs, const X& rhs) noexcept
	{
		return lhs.myValue != rhs;
	}

	template<typename X, typename Y>
	constexpr bool operator!=(const ReadOnly<X*>& lhs, const ReadOnly<Y*>& rhs) noexcept
	{
		return lhs.myValue != rhs.myValue;
	}

	template<typename X, typename Y>
	constexpr bool operator!=(const ReadOnly<X*>& lhs, nullptr_t) noexcept
	{
		return lhs.myValue != nullptr;
	}
}
