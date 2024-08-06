export module Iconer.Utility.Identity;
import <utility>;

export namespace iconer::util
{
	struct Identity final
	{
		template <typename T>
		[[nodiscard]]
		constexpr T&& operator()(T&& value) const noexcept
		{
			return std::forward<T>(value);
		}

		using is_transparent = int;
	};
}
