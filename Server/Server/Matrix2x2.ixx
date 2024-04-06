export module Iconer.Utility.Matrix2x2;
import <utility>;

export namespace iconer::util
{
	struct [[nodiscard]] Matrix2x2 final
	{
		[[nodiscard]]
		constexpr float Determinant() const noexcept
		{
			return _11 * _22 - _12 * _21;
		}

		float _11, _12, _21, _22;
	};
}
