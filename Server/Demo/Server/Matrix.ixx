module;
#include <stdexcept>
#include <type_traits>
#include <concepts>
#include <memory>
#include <initializer_list>
#include <span>
#include <utility>
export module Iconer.Utility.D3D.Matrix;
import Iconer.Utility.Constraints;
export import Iconer.Utility.Matrix3x3;

#define MAKE_SCALAR_REFERENCES(vars_prefix, mat) auto& vars_prefix##_11 = (mat).scalar[0], & vars_prefix##_12 = (mat).scalar[1], & vars_prefix##_13 = (mat).scalar[2], & vars_prefix##_14 = (mat).scalar[3]; \
auto& vars_prefix##_21 = (mat).scalar[4], & vars_prefix##_22 = (mat).scalar[5], & vars_prefix##_23 = (mat).scalar[6], & vars_prefix##_24 = (mat).scalar[7]; \
auto& vars_prefix##_31 = (mat).scalar[8], & vars_prefix##_32 = (mat).scalar[9], & vars_prefix##_33 = (mat).scalar[10], & vars_prefix##_34 = (mat).scalar[11]; \
auto& vars_prefix##_41 = (mat).scalar[12], & vars_prefix##_42 = (mat).scalar[13], & vars_prefix##_43 = (mat).scalar[14], & vars_prefix##_44 = (mat).scalar[15];

#define MAKE_REFERENCES(mat_name) MAKE_SCALAR_REFERENCES(mat_name, mat_name)

export namespace iconer::util::d3d
{
	struct [[nodiscard]] Vector3 final
	{
		float x, y, z;
	};

	struct [[nodiscard]] Vector4 final
	{
		float x, y, z, w;
	};

	struct [[nodiscard]] MAT3X3 final
	{
		[[nodiscard]]
		constexpr float Determinant() const noexcept
		{
			return scalar[0] * scalar[4] * scalar[8] + scalar[1] * scalar[5] * scalar[6] + scalar[2] * scalar[3] * scalar[7] - scalar[0] * scalar[5] * scalar[7] - scalar[1] * scalar[3] * scalar[8] - scalar[2] * scalar[4] * scalar[6];
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

		float scalar[9];
	};

	class [[nodiscard]] MatrixDeterminantError : public std::domain_error
	{
	public:
		using Super = std::domain_error;

		using Super::Super;
	};

	class [[nodiscard]] Matrix final
	{
	public:
		constexpr Matrix GetInverse() const noexcept
		{
			return !(*this);
		}

		constexpr Matrix& Inverse() & noexcept
		{
			return *this = !(*this);
		}

		constexpr Matrix Inverse() && noexcept
		{
			return std::move(*this = !(*this));
		}

		constexpr Matrix& Transpose() & noexcept
		{
			return *this = ~(*this);
		}
		
		[[nodiscard]]
		constexpr Matrix Transpose() && noexcept
		{
			return std::move(*this = ~(*this));
		}

		[[nodiscard]]
		constexpr Matrix GetTranspose() const noexcept
		{
			return ~(*this);
		}

		[[nodiscard]]
		constexpr auto Minor(const size_t& row, const size_t& column) const noexcept
		{
			MAKE_SCALAR_REFERENCES(my, *this);

			//MAT3X3 result{};
			Matrix3x3 result{};
			auto& result_input = result.scalar;
			size_t result_index = 0;

			for (size_t r = 0; r < 4; ++r)
			{
				if (row == r) continue;

				auto current_row = Row(r);

				for (size_t c = 0; c < 4; ++c)
				{
					if (column == c) continue;

					auto& current_scalar = current_row[c];
					result_input[result_index] = current_scalar;

					++result_index;
				}
			}

			return result;
		}

		[[nodiscard]]
		constexpr Matrix Cofactor() const noexcept
		{
			Matrix result{};
			MAKE_SCALAR_REFERENCES(res, result);
			MAKE_SCALAR_REFERENCES(my, *this);

			res_11 = my_11 * Minor(0, 0).Determinant();
			res_12 = -my_12 * Minor(0, 1).Determinant();
			res_13 = my_13 * Minor(0, 2).Determinant();
			res_14 = -my_14 * Minor(0, 3).Determinant();

			res_21 = -my_21 * Minor(1, 0).Determinant();
			res_22 = my_22 * Minor(1, 1).Determinant();
			res_23 = -my_23 * Minor(1, 2).Determinant();
			res_24 = my_24 * Minor(1, 3).Determinant();

			res_31 = my_31 * Minor(2, 0).Determinant();
			res_32 = -my_32 * Minor(2, 1).Determinant();
			res_33 = my_33 * Minor(2, 2).Determinant();
			res_34 = -my_34 * Minor(2, 3).Determinant();

			res_41 = -my_41 * Minor(3, 0).Determinant();
			res_42 = my_42 * Minor(3, 1).Determinant();
			res_43 = -my_43 * Minor(3, 2).Determinant();
			res_44 = my_44 * Minor(3, 3).Determinant();

			return result;
		}

		[[nodiscard]]
		constexpr float Determinant() const
		{
			if (std::is_constant_evaluated())
			{
				MAKE_SCALAR_REFERENCES(my, *this);

				return my_14 * my_23 * my_32 * my_41
					- my_13 * my_24 * my_32 * my_41
					- my_14 * my_22 * my_33 * my_41
					+ my_12 * my_24 * my_33 * my_41
					+ my_13 * my_22 * my_34 * my_41
					- my_12 * my_23 * my_34 * my_41
					- my_31 * my_14 * my_23 * my_42
					+ my_31 * my_13 * my_24 * my_42
					+ my_21 * my_14 * my_33 * my_42
					- my_11 * my_24 * my_33 * my_42
					- my_21 * my_13 * my_34 * my_42
					+ my_11 * my_23 * my_34 * my_42
					+ my_31 * my_14 * my_22 * my_43
					- my_31 * my_12 * my_24 * my_43
					- my_21 * my_14 * my_32 * my_43
					+ my_11 * my_24 * my_32 * my_43
					+ my_21 * my_12 * my_34 * my_43
					- my_11 * my_22 * my_34 * my_43
					- my_31 * my_13 * my_22 * my_44
					+ my_31 * my_12 * my_23 * my_44
					+ my_21 * my_13 * my_32 * my_44
					- my_11 * my_23 * my_32 * my_44
					- my_21 * my_12 * my_33 * my_44
					+ my_11 * my_22 * my_33 * my_44;
			}
			else
			{
				return GetDeterminantImplementation();
			}
		}

		[[nodiscard]]
		constexpr Matrix Copy() noexcept
		{
			MAKE_SCALAR_REFERENCES(my, *this);

			return Matrix
			{
				my_11, my_12, my_13, my_14,
				my_21, my_22, my_23, my_24,
				my_31, my_32, my_33, my_34,
				my_41, my_42, my_43, my_44,
			};
		}

		[[nodiscard]]
		constexpr auto AsSpan() noexcept
		{
			return std::span<float, 16>{ scalar };
		}

		[[nodiscard]]
		constexpr auto AsSpan() const noexcept
		{
			return std::span<const float, 16>{ scalar };
		}

		[[nodiscard]]
		constexpr std::span<float, 4> Row(const size_t nth) noexcept
		{
			return std::span<float, 4>{ scalar + nth * 4, 4  };
		}

		[[nodiscard]]
		constexpr std::span<const float, 4> Row(const size_t nth) const noexcept
		{
			return std::span<const float, 4>{ scalar + nth * 4, 4  };
		}

		template<size_t Index>
		[[nodiscard]]
		constexpr std::span<float, 4> Row() noexcept(Index < 4)
		{
			static_assert(Index < 4);

			return std::span<float, 4>{ scalar + Index * 4, 4  };
		}

		template<size_t Index>
		[[nodiscard]]
		constexpr std::span<const float, 4> Row() const noexcept(Index < 4)
		{
			static_assert(Index < 4);

			return std::span<const float, 4>{ scalar + Index * 4, 4  };
		}

		inline friend constexpr Matrix& operator+=(Matrix& lhs, const Matrix& rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				MAKE_REFERENCES(lhs);
				MAKE_REFERENCES(rhs);

				lhs_11 += rhs_11; lhs_12 += rhs_12; lhs_13 += rhs_13; lhs_14 += rhs_14;
				lhs_21 += rhs_21; lhs_22 += rhs_22; lhs_23 += rhs_23; lhs_24 += rhs_24;
				lhs_31 += rhs_31; lhs_32 += rhs_32; lhs_33 += rhs_33; lhs_34 += rhs_34;
				lhs_41 += rhs_41; lhs_42 += rhs_42; lhs_43 += rhs_43; lhs_44 += rhs_44;
			}
			else
			{
				lhs.LocalAddImplementation(rhs);
			}

			return lhs;
		}

		[[nodiscard]]
		inline friend constexpr Matrix operator+(const Matrix& lhs, const Matrix& rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				Matrix result{};
				MAKE_REFERENCES(result);
				MAKE_REFERENCES(lhs);
				MAKE_REFERENCES(rhs);

				result_11 = lhs_11 + rhs_11; result_12 = lhs_12 + rhs_12;
				result_13 = lhs_13 + rhs_13; result_14 = lhs_14 + rhs_14;
				result_21 = lhs_21 + rhs_21; result_22 = lhs_22 + rhs_22;
				result_23 = lhs_23 + rhs_23; result_24 = lhs_24 + rhs_24;

				result_31 = lhs_31 + rhs_31; result_32 = lhs_32 + rhs_32;
				result_33 = lhs_33 + rhs_33; result_34 = lhs_34 + rhs_34;
				result_41 = lhs_41 + rhs_41; result_42 = lhs_42 + rhs_42;
				result_43 = lhs_43 + rhs_43; result_44 = lhs_44 + rhs_44;

				return result;
			}
			else
			{
				return iconer::util::d3d::Matrix::AddImplementation(lhs, rhs);
			}
		}

		inline friend constexpr Matrix& operator-=(Matrix& lhs, const Matrix& rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				MAKE_REFERENCES(lhs);
				MAKE_REFERENCES(rhs);

				lhs_11 -= rhs_11; lhs_12 -= rhs_12; lhs_13 -= rhs_13; lhs_14 -= rhs_14;
				lhs_21 -= rhs_21; lhs_22 -= rhs_22; lhs_23 -= rhs_23; lhs_24 -= rhs_24;
				lhs_31 -= rhs_31; lhs_32 -= rhs_32; lhs_33 -= rhs_33; lhs_34 -= rhs_34;
				lhs_41 -= rhs_41; lhs_42 -= rhs_42; lhs_43 -= rhs_43; lhs_44 -= rhs_44;
			}
			else
			{
				lhs.LocalSubtractImplementation(rhs);
			}

			return lhs;
		}

		[[nodiscard]]
		inline friend constexpr Matrix operator-(const Matrix& lhs, const Matrix& rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				Matrix result{};
				MAKE_REFERENCES(result);
				MAKE_REFERENCES(lhs);
				MAKE_REFERENCES(rhs);

				result_11 = lhs_11 - rhs_11; result_12 = lhs_12 - rhs_12;
				result_13 = lhs_13 - rhs_13; result_14 = lhs_14 - rhs_14;
				result_21 = lhs_21 - rhs_21; result_22 = lhs_22 - rhs_22;
				result_23 = lhs_23 - rhs_23; result_24 = lhs_24 - rhs_24;

				result_31 = lhs_31 - rhs_31; result_32 = lhs_32 - rhs_32;
				result_33 = lhs_33 - rhs_33; result_34 = lhs_34 - rhs_34;
				result_41 = lhs_41 - rhs_41; result_42 = lhs_42 - rhs_42;
				result_43 = lhs_43 - rhs_43; result_44 = lhs_44 - rhs_44;

				return result;
			}
			else
			{
				return iconer::util::d3d::Matrix::SubtractImplementation(lhs, rhs);
			}
		}

		inline friend constexpr Matrix& operator*=(Matrix& lhs, const Matrix& rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				lhs = lhs * rhs;
			}
			else
			{
				lhs.LocalMultiplyImplementation(rhs);
			}

			return lhs;
		}

		inline friend constexpr Matrix& operator*=(Matrix& lhs, Matrix&& rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				lhs = lhs * std::move(rhs);
			}
			else
			{
				lhs.LocalMultiplyImplementation(std::move(rhs));
			}

			return lhs;
		}

		template<typename T>
			requires (not same_as<clean_t<T>, Matrix>)
		constexpr Matrix& operator*=(T&& rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				MAKE_SCALAR_REFERENCES(lhs, (*this));

				lhs_11 *= rhs;
				lhs_12 *= rhs;
				lhs_13 *= rhs;
				lhs_14 *= rhs;

				lhs_21 *= rhs;
				lhs_22 *= rhs;
				lhs_23 *= rhs;
				lhs_24 *= rhs;

				lhs_31 *= rhs;
				lhs_32 *= rhs;
				lhs_33 *= rhs;
				lhs_34 *= rhs;

				lhs_41 *= rhs;
				lhs_42 *= rhs;
				lhs_43 *= rhs;
				lhs_44 *= rhs;
			}
			else
			{
				LocalMultiplyImplementation(std::forward<T>(rhs));
			}

			return *this;
		}

		[[nodiscard]]
		inline friend constexpr Matrix operator*(const Matrix& lhs, const Matrix& rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				Matrix result{};
				MAKE_REFERENCES(result);
				MAKE_REFERENCES(lhs);
				MAKE_REFERENCES(rhs);

				result_11 = lhs_11 * rhs_11 + lhs_12 * rhs_21 + lhs_13 * rhs_31 + lhs_14 * rhs_41;
				result_21 = lhs_21 * rhs_11 + lhs_22 * rhs_21 + lhs_23 * rhs_31 + lhs_24 * rhs_41;
				result_31 = lhs_31 * rhs_11 + lhs_32 * rhs_21 + lhs_33 * rhs_31 + lhs_34 * rhs_41;
				result_41 = lhs_41 * rhs_11 + lhs_42 * rhs_21 + lhs_43 * rhs_31 + lhs_44 * rhs_41;

				result_12 = lhs_11 * rhs_12 + lhs_12 * rhs_22 + lhs_13 * rhs_32 + lhs_14 * rhs_42;
				result_22 = lhs_21 * rhs_12 + lhs_22 * rhs_22 + lhs_23 * rhs_32 + lhs_24 * rhs_42;
				result_32 = lhs_31 * rhs_12 + lhs_32 * rhs_22 + lhs_33 * rhs_32 + lhs_34 * rhs_42;
				result_42 = lhs_41 * rhs_12 + lhs_42 * rhs_22 + lhs_43 * rhs_32 + lhs_44 * rhs_42;

				result_13 = lhs_11 * rhs_13 + lhs_12 * rhs_23 + lhs_13 * rhs_33 + lhs_14 * rhs_43;
				result_23 = lhs_21 * rhs_13 + lhs_22 * rhs_23 + lhs_23 * rhs_33 + lhs_24 * rhs_43;
				result_43 = lhs_41 * rhs_13 + lhs_42 * rhs_23 + lhs_43 * rhs_33 + lhs_44 * rhs_43;

				result_14 = lhs_11 * rhs_14 + lhs_12 * rhs_24 + lhs_13 * rhs_34 + lhs_14 * rhs_44;
				result_24 = lhs_21 * rhs_14 + lhs_22 * rhs_24 + lhs_23 * rhs_34 + lhs_24 * rhs_44;
				result_34 = lhs_31 * rhs_14 + lhs_32 * rhs_24 + lhs_33 * rhs_34 + lhs_34 * rhs_44;
				result_44 = lhs_41 * rhs_14 + lhs_42 * rhs_24 + lhs_43 * rhs_34 + lhs_44 * rhs_44;

				return result;
			}
			else
			{
				return iconer::util::d3d::Matrix::MultiplyImplementation(lhs, rhs);
			}
		}

		[[nodiscard]]
		inline friend constexpr Matrix operator*(const Matrix& lhs, Matrix&& rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				Matrix result{};
				Matrix mid = std::move(rhs);

				MAKE_REFERENCES(result);
				MAKE_REFERENCES(lhs);
				MAKE_REFERENCES(mid);

				result_11 = lhs_11 * mid_11 + lhs_12 * mid_21 + lhs_13 * mid_31 + lhs_14 * mid_41;
				result_21 = lhs_21 * mid_11 + lhs_22 * mid_21 + lhs_23 * mid_31 + lhs_24 * mid_41;
				result_31 = lhs_31 * mid_11 + lhs_32 * mid_21 + lhs_33 * mid_31 + lhs_34 * mid_41;
				result_41 = lhs_41 * mid_11 + lhs_42 * mid_21 + lhs_43 * mid_31 + lhs_44 * mid_41;

				result_12 = lhs_11 * mid_12 + lhs_12 * mid_22 + lhs_13 * mid_32 + lhs_14 * mid_42;
				result_22 = lhs_21 * mid_12 + lhs_22 * mid_22 + lhs_23 * mid_32 + lhs_24 * mid_42;
				result_32 = lhs_31 * mid_12 + lhs_32 * mid_22 + lhs_33 * mid_32 + lhs_34 * mid_42;
				result_42 = lhs_41 * mid_12 + lhs_42 * mid_22 + lhs_43 * mid_32 + lhs_44 * mid_42;

				result_13 = lhs_11 * mid_13 + lhs_12 * mid_23 + lhs_13 * mid_33 + lhs_14 * mid_43;
				result_23 = lhs_21 * mid_13 + lhs_22 * mid_23 + lhs_23 * mid_33 + lhs_24 * mid_43;
				result_43 = lhs_41 * mid_13 + lhs_42 * mid_23 + lhs_43 * mid_33 + lhs_44 * mid_43;

				result_14 = lhs_11 * mid_14 + lhs_12 * mid_24 + lhs_13 * mid_34 + lhs_14 * mid_44;
				result_24 = lhs_21 * mid_14 + lhs_22 * mid_24 + lhs_23 * mid_34 + lhs_24 * mid_44;
				result_34 = lhs_31 * mid_14 + lhs_32 * mid_24 + lhs_33 * mid_34 + lhs_34 * mid_44;
				result_44 = lhs_41 * mid_14 + lhs_42 * mid_24 + lhs_43 * mid_34 + lhs_44 * mid_44;
				return result;
			}
			else
			{
				return iconer::util::d3d::Matrix::MultiplyImplementation(lhs, std::move(rhs));
			}
		}

		[[nodiscard]]
		inline friend constexpr Matrix operator*(Matrix&& lhs, Matrix&& rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				Matrix result{};
				Matrix beg = std::move(lhs);
				Matrix mid = std::move(rhs);

				MAKE_REFERENCES(result);
				MAKE_REFERENCES(beg);
				MAKE_REFERENCES(mid);

				result_11 = beg_11 * mid_11 + beg_12 * mid_21 + beg_13 * mid_31 + beg_14 * mid_41;
				result_21 = beg_21 * mid_11 + beg_22 * mid_21 + beg_23 * mid_31 + beg_24 * mid_41;
				result_31 = beg_31 * mid_11 + beg_32 * mid_21 + beg_33 * mid_31 + beg_34 * mid_41;
				result_41 = beg_41 * mid_11 + beg_42 * mid_21 + beg_43 * mid_31 + beg_44 * mid_41;

				result_12 = beg_11 * mid_12 + beg_12 * mid_22 + beg_13 * mid_32 + beg_14 * mid_42;
				result_22 = beg_21 * mid_12 + beg_22 * mid_22 + beg_23 * mid_32 + beg_24 * mid_42;
				result_32 = beg_31 * mid_12 + beg_32 * mid_22 + beg_33 * mid_32 + beg_34 * mid_42;
				result_42 = beg_41 * mid_12 + beg_42 * mid_22 + beg_43 * mid_32 + beg_44 * mid_42;

				result_13 = beg_11 * mid_13 + beg_12 * mid_23 + beg_13 * mid_33 + beg_14 * mid_43;
				result_23 = beg_21 * mid_13 + beg_22 * mid_23 + beg_23 * mid_33 + beg_24 * mid_43;
				result_43 = beg_41 * mid_13 + beg_42 * mid_23 + beg_43 * mid_33 + beg_44 * mid_43;

				result_14 = beg_11 * mid_14 + beg_12 * mid_24 + beg_13 * mid_34 + beg_14 * mid_44;
				result_24 = beg_21 * mid_14 + beg_22 * mid_24 + beg_23 * mid_34 + beg_24 * mid_44;
				result_34 = beg_31 * mid_14 + beg_32 * mid_24 + beg_33 * mid_34 + beg_34 * mid_44;
				result_44 = beg_41 * mid_14 + beg_42 * mid_24 + beg_43 * mid_34 + beg_44 * mid_44;
				return result;
			}
			else
			{
				return iconer::util::d3d::Matrix::MultiplyImplementation(std::move(lhs), std::move(rhs));
			}
		}

		[[nodiscard]]
		inline friend constexpr Matrix operator*(const Matrix& lhs, const float& rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				MAKE_REFERENCES(lhs);

				return iconer::util::d3d::Matrix
				{
					lhs_11 * rhs, lhs_12 * rhs, lhs_13 * rhs, lhs_14 * rhs,
					lhs_21 * rhs, lhs_22 * rhs, lhs_23 * rhs, lhs_14 * rhs,
					lhs_31 * rhs, lhs_32 * rhs, lhs_33 * rhs, lhs_34 * rhs,
					lhs_41 * rhs, lhs_42 * rhs, lhs_43 * rhs, lhs_44 * rhs,
				};
			}
			else
			{
				return iconer::util::d3d::Matrix::MultiplyImplementation(lhs, rhs);
			}
		}

		inline friend constexpr Matrix& operator/=(Matrix& lhs, const float& rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				MAKE_REFERENCES(lhs);

				lhs_11 /= rhs;
				lhs_12 /= rhs;
				lhs_13 /= rhs;
				lhs_14 /= rhs;

				lhs_21 /= rhs;
				lhs_22 /= rhs;
				lhs_23 /= rhs;
				lhs_24 /= rhs;

				lhs_31 /= rhs;
				lhs_32 /= rhs;
				lhs_33 /= rhs;
				lhs_34 /= rhs;

				lhs_41 /= rhs;
				lhs_42 /= rhs;
				lhs_43 /= rhs;
				lhs_44 /= rhs;
			}
			else
			{
				lhs.LocalDivideImplementation(rhs);
			}

			return lhs;
		}

		[[nodiscard]]
		inline friend constexpr Matrix operator/(const Matrix& lhs, const float& rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				MAKE_REFERENCES(lhs);

				return iconer::util::d3d::Matrix
				{
					lhs_11 / rhs, lhs_12 / rhs, lhs_13 / rhs, lhs_14 / rhs,
					lhs_21 / rhs, lhs_22 / rhs, lhs_23 / rhs, lhs_14 / rhs,
					lhs_31 / rhs, lhs_32 / rhs, lhs_33 / rhs, lhs_34 / rhs,
					lhs_41 / rhs, lhs_42 / rhs, lhs_43 / rhs, lhs_44 / rhs,
				};
			}
			else
			{
				return iconer::util::d3d::Matrix::DivideImplementation(lhs, rhs);
			}
		}

		/// <returns>Inverted matrix</returns>
		[[nodiscard]]
		inline friend constexpr Matrix operator!(const Matrix& lhs)
		{
			Matrix result{};
			if (std::is_constant_evaluated())
			{
				const float det = lhs.Determinant();
				if (0 == det)
				{
					throw MatrixDeterminantError{ "Has no inverted matrix." };
				}

				auto adjoint = lhs.Cofactor().Transpose();
				
				return adjoint / det;
			}
			else
			{
				result.InverseImplementation();
			}

			return result;
		}

		/// <returns>Transposed matrix</returns>
		[[nodiscard]]
		inline friend constexpr Matrix operator~(const Matrix& lhs) noexcept
		{
			Matrix result = lhs;
			if (std::is_constant_evaluated())
			{
				std::swap(result.At(1, 0), result.At(0, 1));
				std::swap(result.At(2, 0), result.At(0, 2));
				std::swap(result.At(3, 0), result.At(0, 3));

				std::swap(result.At(0, 1), result.At(1, 0));
				std::swap(result.At(2, 1), result.At(1, 2));
				std::swap(result.At(3, 1), result.At(1, 3));

				std::swap(result.At(0, 2), result.At(2, 0));
				std::swap(result.At(1, 2), result.At(2, 1));
				std::swap(result.At(3, 2), result.At(2, 3));

				std::swap(result.At(0, 3), result.At(3, 0));
				std::swap(result.At(1, 3), result.At(3, 1));
				std::swap(result.At(2, 3), result.At(3, 2));
			}
			else
			{
				result.TransposeImplementation();
			}

			return result;
		}

		/// <returns>Sign-inverted matrix</returns>
		[[nodiscard]]
		inline friend constexpr Matrix operator-(const Matrix& lhs) noexcept
		{
			Matrix result = lhs;
			for (auto& scalar : result.scalar)
			{
				scalar *= -1;
			}

			return result;
		}

		/// <returns>Sign-inverted matrix</returns>
		[[nodiscard]]
		inline friend constexpr Matrix operator-(Matrix&& lhs) noexcept
		{
			Matrix result = std::move(lhs);
			for (auto& scalar : result.scalar)
			{
				scalar *= -1;
			}

			return result;
		}

		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr float& At(const int& index)&
		{
			if (index < 0 or Size() <= index)
			{
				ThrowIndexError();
			}

			return scalar[index];
		}

		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr const float& At(const int& index) const&
		{
			if (index < 0 or Size() <= index)
			{
				ThrowIndexError();
			}

			return scalar[index];
		}

		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr float&& At(const int& index)&&
		{
			if (index < 0 or Size() <= index)
			{
				ThrowIndexError();
			}

			return std::move(scalar[index]);
		}

		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr const float&& At(const int& index) const&&
		{
			if (index < 0 or Size() <= index)
			{
				ThrowIndexError();
			}

			return std::move(scalar[index]);
		}

		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		/// <param name="index">row, column</param>
		constexpr float& At(const std::pair<int, int>& index)&
		{
			if (std::get<0>(index) < 0 or RowSize() <= std::get<0>(index)
				or std::get<1>(index) < 0 or ColumnSize() <= std::get<1>(index))
			{
				ThrowIndexError();
			}

			return scalar[std::get<0>(index) * 4 + std::get<1>(index)];
		}

		/// <param name="index">row, column</param>
		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr const float& At(const std::pair<int, int>& index) const&
		{
			if (std::get<0>(index) < 0 or RowSize() <= std::get<0>(index)
				or std::get<1>(index) < 0 or ColumnSize() <= std::get<1>(index))
			{
				ThrowIndexError();
			}

			return scalar[std::get<0>(index) * 4 + std::get<1>(index)];
		}

		/// <param name="index">row, column</param>
		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr float&& At(const std::pair<int, int>& index)&&
		{
			if (std::get<0>(index) < 0 or RowSize() <= std::get<0>(index)
				or std::get<1>(index) < 0 or ColumnSize() <= std::get<1>(index))
			{
				ThrowIndexError();
			}

			return std::move(scalar[std::get<0>(index) * 4 + std::get<1>(index)]);
		}

		/// <param name="index">row, column</param>
		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr const float&& At(const std::pair<int, int>& index) const&&
		{
			if (std::get<0>(index) < 0 or RowSize() <= std::get<0>(index)
				or std::get<1>(index) < 0 or ColumnSize() <= std::get<1>(index))
			{
				ThrowIndexError();
			}

			return std::move(scalar[std::get<0>(index) * 4 + std::get<1>(index)]);
		}

		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr float& At(const int& column, const int& row)&
		{
			if (row < 0 or RowSize() <= row
				or column < 0 or ColumnSize() <= column)
			{
				ThrowIndexError();
			}

			return scalar[row * 4 + column];
		}

		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr const float& At(const int& column, const int& row) const&
		{
			if (row < 0 or RowSize() <= row
				or column < 0 or ColumnSize() <= column)
			{
				ThrowIndexError();
			}

			return scalar[row * 4 + column];
		}

		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr float&& At(const int& column, const int& row)&&
		{
			if (row < 0 or RowSize() <= row
				or column < 0 or ColumnSize() <= column)
			{
				ThrowIndexError();
			}

			return std::move(scalar[row * 4 + column]);
		}

		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr const float&& At(const int& column, const int& row) const&&
		{
			if (row < 0 or RowSize() <= row
				or column < 0 or ColumnSize() <= column)
			{
				ThrowIndexError();
			}

			return std::move(scalar[row * 4 + column]);
		}

		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr float& operator[](const int& index)&
		{
			if (index < 0 or Size() <= index)
			{
				ThrowIndexError();
			}

			return scalar[index];
		}

		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr const float& operator[](const int& index) const&
		{
			if (index < 0 or Size() <= index)
			{
				ThrowIndexError();
			}

			return scalar[index];
		}

		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr float&& operator[](const int& index)&&
		{
			if (index < 0 or Size() <= index)
			{
				ThrowIndexError();
			}

			return std::move(scalar[index]);
		}

		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr const float&& operator[](const int& index) const&&
		{
			if (index < 0 or Size() <= index)
			{
				ThrowIndexError();
			}

			return std::move(scalar[index]);
		}

		/// <param name="index">row, column</param>
		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr float& operator[](const std::pair<int, int>& index)&
		{
			if (std::get<0>(index) < 0 or RowSize() <= std::get<0>(index)
				or std::get<1>(index) < 0 or ColumnSize() <= std::get<1>(index))
			{
				ThrowIndexError();
			}

			return scalar[std::get<0>(index) * 4 + std::get<1>(index)];
		}

		/// <param name="index">row, column</param>
		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr const float& operator[](const std::pair<int, int>& index) const&
		{
			if (std::get<0>(index) < 0 or RowSize() <= std::get<0>(index)
				or std::get<1>(index) < 0 or ColumnSize() <= std::get<1>(index))
			{
				ThrowIndexError();
			}

			return scalar[std::get<0>(index) * 4 + std::get<1>(index)];
		}

		/// <param name="index">row, column</param>
		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr float&& operator[](const std::pair<int, int>& index)&&
		{
			if (std::get<0>(index) < 0 or RowSize() <= std::get<0>(index)
				or std::get<1>(index) < 0 or ColumnSize() <= std::get<1>(index))
			{
				ThrowIndexError();
			}

			return std::move(scalar[std::get<0>(index) * 4 + std::get<1>(index)]);
		}

		/// <param name="index">row, column</param>
		/// <exception cref="std::out_of_range"/>
		[[nodiscard]]
		constexpr const float&& operator[](const std::pair<int, int>& index) const&&
		{
			if (std::get<0>(index) < 0 or RowSize() <= std::get<0>(index)
				or std::get<1>(index) < 0 or ColumnSize() <= std::get<1>(index))
			{
				ThrowIndexError();
			}

			return std::move(scalar[std::get<0>(index) * 4 + std::get<1>(index)]);
		}

		[[nodiscard]]
		constexpr size_t Size() const noexcept
		{
			return 16;
		}

		[[nodiscard]]
		constexpr size_t RowSize() const noexcept
		{
			return 4;
		}

		[[nodiscard]]
		constexpr size_t ColumnSize() const noexcept
		{
			return 4;
		}

		[[nodiscard]]
		constexpr ptrdiff_t SignedSize() const noexcept
		{
			return 16;
		}

		[[nodiscard]]
		constexpr ptrdiff_t SignedRowSize() const noexcept
		{
			return 4;
		}

		[[nodiscard]]
		constexpr ptrdiff_t SignedColumnSize() const noexcept
		{
			return 4;
		}

		[[nodiscard]]
		static consteval size_t MaxSize() noexcept
		{
			return 16;
		}

		[[nodiscard]]
		static consteval size_t MaxRowSize() noexcept
		{
			return 4;
		}

		[[nodiscard]]
		static consteval size_t MaxColumnSize() noexcept
		{
			return 4;
		}

		float scalar[16];

	private:
		[[noreturn]]
		static void ThrowIndexError()
		{
			throw std::out_of_range{ "Index is out of range." };
		}

		void LocalAddImplementation(const Matrix& rhs) noexcept;
		[[nodiscard]] static Matrix AddImplementation(const Matrix& lhs, const Matrix& rhs) noexcept;
		void LocalSubtractImplementation(const Matrix& rhs) noexcept;
		[[nodiscard]] static Matrix SubtractImplementation(const Matrix& lhs, const Matrix& rhs) noexcept;

		void LocalMultiplyImplementation(const Matrix& rhs) noexcept;
		void LocalMultiplyImplementation(Matrix&& rhs) noexcept;
		void LocalMultiplyImplementation(const float& rhs) noexcept;
		void LocalMultiplyImplementation(const double& rhs) noexcept;
		void LocalMultiplyImplementation(float&& rhs) noexcept;
		void LocalMultiplyImplementation(double&& rhs) noexcept;
		[[nodiscard]] static Matrix MultiplyImplementation(const Matrix& lhs, const Matrix& rhs) noexcept;
		[[nodiscard]] static Matrix MultiplyImplementation(const Matrix& lhs, Matrix&& rhs) noexcept;
		[[nodiscard]] static Matrix MultiplyImplementation(Matrix&& lhs, Matrix&& rhs) noexcept;
		[[nodiscard]] static Matrix MultiplyImplementation(const Matrix& lhs, const float& value) noexcept;
		[[nodiscard]] static Matrix MultiplyImplementation(const Matrix& lhs, float&& value) noexcept;
		[[nodiscard]] static Matrix MultiplyImplementation(const Matrix& lhs, const double& value) noexcept;
		[[nodiscard]] static Matrix MultiplyImplementation(const Matrix& lhs, double&& value) noexcept;

		void LocalDivideImplementation(const float& value) noexcept;
		void LocalDivideImplementation(const double& value) noexcept;
		void LocalDivideImplementation(float&& value) noexcept;
		void LocalDivideImplementation(double&& value) noexcept;
		[[nodiscard]] static Matrix DivideImplementation(const Matrix& lhs, const float& value) noexcept;
		[[nodiscard]] static Matrix DivideImplementation(const Matrix& lhs, const double& value) noexcept;
		[[nodiscard]] static Matrix DivideImplementation(const Matrix& lhs, float&& value) noexcept;
		[[nodiscard]] static Matrix DivideImplementation(const Matrix& lhs, double&& value) noexcept;

		void TransposeImplementation() noexcept;
		[[nodiscard]] void InverseImplementation();
		[[nodiscard]] float GetDeterminantImplementation() const;
	};

	inline constexpr Matrix ZeroMatrix
	{
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	};
	inline constexpr Matrix IdentityMatrix
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	struct [[nodiscard]] Quaternion final
	{
		float x, y, z, w;
	};
}

module :private;

namespace iconer::util::d3d::test
{
	constexpr Matrix TEST_00() noexcept
	{
		Matrix mat1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
		return mat1 *= IdentityMatrix;
	}

	constexpr auto TEST_01() noexcept
	{
		Matrix mat1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
		mat1 *= IdentityMatrix;
		return mat1[{3, 0}];
	}

	constexpr auto TEST_02() noexcept
	{
		Matrix mat1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
		Matrix mat2{ 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

		mat1 *= mat2;

		auto span = mat1.AsSpan();
		return span[2];
	}

	constexpr auto TEST_03() noexcept
	{
		Matrix mat1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
		Matrix mat2{ 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

		auto r = mat1 * mat2;

		auto span = r.AsSpan();
		return span[2];
	}

	constexpr auto TEST_04() noexcept
	{
		Matrix mat1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
		Matrix mat2{ 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

		mat1 *= mat2;
		return mat1;
	}

	constexpr auto TEST_05() noexcept
	{
		Matrix mat1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

		mat1 *= IdentityMatrix;
		return mat1;
	}

	constexpr auto TEST_06() noexcept
	{
		Matrix mat1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

		auto row = mat1.Row(2);

		return row[0];
	}

	constexpr auto TEST_07() noexcept
	{
		Matrix mat1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

		auto row = mat1.Row<3>();

		return row[2];
	}

	constexpr auto TEST_08() noexcept
	{
		Matrix mat1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

		return ~mat1;
	}

	constexpr auto TEST_09() noexcept
	{
		Matrix mat1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

		return -mat1;
	}

	constexpr auto TEST_10() noexcept
	{
		Matrix mat1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

		return mat1.Determinant();
	}

	constexpr auto TEST_11() noexcept
	{
		Matrix mat1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

		return mat1.Minor(0, 0);
	}

	constexpr auto TEST_12() noexcept
	{
		Matrix mat1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

		return mat1.Minor(2, 2);
	}

	constexpr auto TEST_13() noexcept
	{
		Matrix mat1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

		return mat1.Minor(1, 2);
	}

	constexpr void testments()
	{
		constexpr Matrix mat0{ };
		constexpr Matrix mat1
		{
			 1,  2,  3,  4,
			 5,  6,  7,  8,
			 9, 10, 11, 12,
			13, 14, 15, 16
		};
		constexpr Matrix mat2{ 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

		constexpr Matrix mat_res0 = mat1 * IdentityMatrix;
		constexpr Matrix mat_res0_r = TEST_00();
		constexpr Matrix mat_res1 = mat2 / 2;
		constexpr Matrix mat_res2 = mat1 + mat2;
		constexpr Matrix mat_res3 = mat1 - mat2;
		constexpr Matrix mat_res4 = mat1 * 10;
		constexpr Matrix mat_res5 = mat1 * mat2;
		constexpr Matrix mat_res6 = mat2 * mat1;
		constexpr Matrix mat_res7 = mat2 * (IdentityMatrix * 2);
		constexpr Matrix mat_res8 = (IdentityMatrix * 2) * mat1;
		constexpr auto mat_res9 = TEST_01();

		constexpr auto mat_res10 = TEST_02();
		constexpr auto mat_res11 = TEST_03();

		constexpr auto mat_res12 = TEST_04();
		constexpr auto mat_res13 = TEST_05();

		constexpr auto mat_res14 = TEST_06();
		constexpr auto mat_res15 = TEST_07();

		constexpr auto mat_res16 = TEST_08();
		constexpr auto mat_res17 = TEST_09();
		constexpr auto mat_res18 = TEST_10();

		constexpr auto mat_res19 = TEST_11();
		constexpr auto mat_res20 = TEST_12();
		constexpr auto mat_res21 = TEST_13();

		constexpr auto mat_res22 = (IdentityMatrix * 2).Determinant();
		constexpr auto mat_res23 = ~IdentityMatrix;
		constexpr auto mat_res24 = IdentityMatrix.Determinant();
		constexpr auto mat_res25 = IdentityMatrix.Cofactor();
		constexpr auto mat_res26 = IdentityMatrix.Minor(0, 2);

		constexpr auto mat_res27 = Matrix{ 12, 0, 0, 1, 4, -6, 0, 31, 10, 7, 3, 0, 0, 25, 2, 7 }.Cofactor();
		constexpr auto mat_res28 = Matrix{ 12, 0, 0, 1, 4, -6, 0, 31, 10, 7, 3, 0, 0, 25, 2, 7 }.Determinant();
		constexpr auto mat_res29 = !Matrix{ 12, 0, 0, 1, 4, -6, 0, 31, 10, 7, 3, 0, 0, 25, 2, 7 };
		constexpr auto mat_res30 = (Matrix{ 12, 0, 0, 1, 4, -6, 0, 31, 10, 7, 3, 0, 0, 25, 2, 7 }) * (!Matrix{ 12, 0, 0, 1, 4, -6, 0, 31, 10, 7, 3, 0, 0, 25, 2, 7 });
		constexpr auto mat_res31 = (!Matrix{ 12, 0, 0, 1, 4, -6, 0, 31, 10, 7, 3, 0, 0, 25, 2, 7 }) * (Matrix{ 12, 0, 0, 1, 4, -6, 0, 31, 10, 7, 3, 0, 0, 25, 2, 7 });

		constexpr auto mat_res32 = IdentityMatrix.GetInverse();
		constexpr auto mat_res33 = !IdentityMatrix;
	}
}
