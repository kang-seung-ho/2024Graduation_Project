export module Iconer.Utility.D3D.Transform;
export import Iconer.Utility.D3D.Matrix;
export import :XYZWrapper;
import <type_traits>;

export namespace iconer::util::d3d
{
	class Transform
	{
	public:
		constexpr Transform& SetMatrix(const Matrix& mat) noexcept
		{
			myMatrix = mat;
			return *this;
		}

		Transform& SetMatrix(Matrix&& mat) noexcept
		{
			myMatrix = std::move(mat);
			return *this;
		}

		Transform& SetScale(float x, float y, float z) noexcept
		{
			myMatrix *= x;
			return *this;
		}

		constexpr Transform& SetPosition(float x, float y, float z) noexcept
		{
			return *this;
		}

		constexpr Transform& SetPosition(const Vector3& pos) noexcept
		{
			return *this;
		}

		constexpr Transform& SetPosition(Vector3&& pos) noexcept
		{
			return *this;
		}

		constexpr Transform& SetRotation(const Matrix& tfrm) noexcept
		{
			return *this;
		}

		constexpr Transform& SetRotation(Matrix&& tfrm) noexcept
		{
			return *this;
		}

		Transform& Translate(float x, float y, float z) noexcept;
		Transform& Translate(const Vector3& shift) noexcept;
		Transform& Translate(Vector3&& shift) noexcept;
		Transform& Move(const Vector3& dir, float distance) noexcept;
		Transform& Move(Vector3&& dir, float distance) noexcept;
		Transform& MoveStrafe(float distance) noexcept;
		Transform& MoveForward(float distance) noexcept;
		Transform& MoveUp(float distance) noexcept;

		Transform& Rotate(const Matrix& tfrm) noexcept;
		Transform& Rotate(Matrix&& tfrm) noexcept;
		Transform& Rotate(float pitch, float yaw, float roll) noexcept;
		Transform& Rotate(const Vector3& axis, float angle) noexcept;
		Transform& Rotate(const Quaternion& quaternion) noexcept;
		Transform& Rotate(Quaternion&& quaternion) noexcept;

		Transform& LookTo(const Vector3& look, const Vector3& up) noexcept;
		Transform& LookAt(const Vector3& look, const Vector3& up) noexcept;

		[[nodiscard]] Matrix& GetMatrix() noexcept;
		[[nodiscard]] const Matrix& GetMatrix() const;
		[[nodiscard]] XYZWrapper& GetRight() noexcept;
		[[nodiscard]] XYZWrapper& GetUp() noexcept;
		[[nodiscard]] XYZWrapper& GetLook() noexcept;
		[[nodiscard]] XYZWrapper& GetPosition() noexcept;
		[[nodiscard]] const XYZWrapper& GetRight() const noexcept;
		[[nodiscard]] const XYZWrapper& GetUp() const noexcept;
		[[nodiscard]] const XYZWrapper& GetLook() const noexcept;
		[[nodiscard]] const XYZWrapper& GetPosition() const noexcept;

		Matrix myMatrix;

		XYZWrapper myRight{ myMatrix.At(0, 0), myMatrix.At(0, 1), myMatrix.At(0, 2) };
		XYZWrapper myUp{ myMatrix.At(1, 0), myMatrix.At(1, 1), myMatrix.At(1, 2) };
		XYZWrapper myLook{ myMatrix.At(2, 0), myMatrix.At(2, 1), myMatrix.At(2, 2) };
		XYZWrapper myPosition{ myMatrix.At(3, 0), myMatrix.At(3, 1), myMatrix.At(3, 2) };

		static inline constexpr Vector3 Up = Vector3(0.0f, 1.0f, 0.0f);
		static inline constexpr Vector3 Forward = Vector3(0.0f, 0.0f, 1.0f);
		static inline constexpr Vector3 Right = Vector3(1.0f, 0.0f, 0.0f);

	private:
		bool hasUpdated;
	};
}
