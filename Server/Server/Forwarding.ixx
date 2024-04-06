export module Iconer.Utility.Forwarding;
import <utility>;

export namespace iconer::util
{
	struct CopyToForward
	{
		template <typename T>
		[[nodiscard]]
		constexpr std::remove_reference_t<T> operator()(T&& value) const noexcept(std::is_nothrow_copy_constructible_v<T>)
		{
			return value;
		}

		using is_transparent = int;
	};

	struct MoveToForward
	{
		template <typename T>
		[[nodiscard]]
		constexpr std::remove_reference_t<T>&& operator()(std::remove_reference_t<T>& value) const noexcept(std::is_nothrow_move_constructible_v<T>)
		{
			return static_cast<std::remove_reference_t<T>&&>(value);
		}
		
		template <typename T>
		[[nodiscard]]
		constexpr std::remove_reference_t<T>&& operator()(std::remove_reference_t<T>&& value) const noexcept(std::is_nothrow_move_constructible_v<T>)
		{
			return static_cast<std::remove_reference_t<T>&&>(value);
		}

		using is_transparent = int;
	};
}
