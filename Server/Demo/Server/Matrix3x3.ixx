export module Iconer.Utility.Matrix3x3;
import <utility>;
import <span>;

export namespace iconer::util
{
	class [[nodiscard]] Matrix3x3 final
	{
	public:
		[[nodiscard]]
		constexpr Matrix3x3 Cofactor() const noexcept
		{
			Matrix3x3 result{};
			result.scalar[0] = 1 * (scalar[4] * scalar[8] - scalar[5] * scalar[7]);
			result.scalar[1] = -1 * (scalar[3] * scalar[8] - scalar[5] * scalar[6]);
			result.scalar[2] = 1 * (scalar[3] * scalar[7] - scalar[4] * scalar[6]);

			result.scalar[3] = -1 * (scalar[1] * scalar[8] - scalar[2] * scalar[7]);
			result.scalar[4] = 1 * (scalar[0] * scalar[8] - scalar[2] * scalar[6]);
			result.scalar[5] = -1 * (scalar[0] * scalar[7] - scalar[1] * scalar[6]);

			result.scalar[3] = 1 * (scalar[1] * scalar[5] - scalar[2] * scalar[4]);
			result.scalar[4] = -1 * (scalar[0] * scalar[5] - scalar[2] * scalar[3]);
			result.scalar[5] = 1 * (scalar[0] * scalar[4] - scalar[1] * scalar[3]);

			return result;
		}

		[[nodiscard]]
		constexpr float Determinant() const noexcept
		{
			if (std::is_constant_evaluated())
			{
				return scalar[0] * scalar[4] * scalar[8] + scalar[1] * scalar[5] * scalar[6] + scalar[2] * scalar[3] * scalar[7] - scalar[0] * scalar[5] * scalar[7] - scalar[1] * scalar[3] * scalar[8] - scalar[2] * scalar[4] * scalar[6];
			}
			else
			{
				return GetDeterminantImplementation();
			}
		}

		/// <returns>Transposed matrix</returns>
		[[nodiscard]]
		inline friend constexpr Matrix3x3 operator~(const Matrix3x3& lhs) noexcept
		{
			Matrix3x3 result = lhs;

			std::swap(result.scalar[1], result.scalar[3]);
			std::swap(result.scalar[2], result.scalar[6]);
			std::swap(result.scalar[5], result.scalar[7]);

			return result;
		}

		[[nodiscard]]
		constexpr std::span<float, 9> AsSpan() noexcept
		{
			return std::span<float, 9>{ scalar, 9 };
		}

		[[nodiscard]]
		constexpr std::span<const float, 9> AsSpan() const noexcept
		{
			return std::span<const float, 9>{ scalar, 9 };
		}

		[[nodiscard]]
		constexpr std::span<float, 3> Row(const size_t nth) noexcept
		{
			return std::span<float, 3>{ scalar + nth * 3, 3  };
		}

		[[nodiscard]]
		constexpr std::span<const float, 3> Row(const size_t nth) const noexcept
		{
			return std::span<const float, 3>{ scalar + nth * 3, 3  };
		}

		template<size_t Index>
		[[nodiscard]]
		constexpr std::span<float, 3> Row() noexcept(Index < 3)
		{
			static_assert(Index < 3);

			return std::span<float, 3>{ scalar + Index * 3, 3  };
		}

		template<size_t Index>
		[[nodiscard]]
		constexpr std::span<const float, 3> Row() const noexcept(Index < 3)
		{
			static_assert(Index < 3);

			return std::span<const float, 3>{ scalar + Index * 3, 3  };
		}

		float scalar[9];

	private:
		[[nodiscard]] float GetDeterminantImplementation() const noexcept;
	};
}

module :private;

namespace iconer::util::test
{
	//*
	void testments()
	{
		constexpr auto mat_res0 = Matrix3x3{ 10, 0, 0, 0, 10, 0, 0, 0, 10 }.Determinant();
		constexpr auto mat_res1 = Matrix3x3{ 100, 0, 0, 0, 100, 0, 0, 0, 100 }.Determinant();
		constexpr auto mat_res2 = Matrix3x3{ 100, 0, 0, 0, 100, 0, 0, 0, 100 }.Cofactor();
		constexpr auto mat_res3 = Matrix3x3{ 100, 0, 0, 0, 100, 0, 0, 0, 100 }.Cofactor().Determinant();


		constexpr auto mat_res4 = ~Matrix3x3{ 10, 0, 0, 0, 10, 0, 0, 0, 10 };
		constexpr auto mat_res5 = ~Matrix3x3
		{
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		};
	}
	//*/
}
