module;
#include <utility>
#include <tuple>
#include <span>

export module Iconer.Utility.D3D.Transform:XYZWrapper;
export import Iconer.Utility.D3D.Matrix;

export namespace iconer::util::d3d
{
	class [[nodiscard]] XYZWrapper final
	{
	public:
		explicit constexpr XYZWrapper(float& x, float& y, float& z) noexcept
			: x(x), y(y), z(z)
		{
		}

		explicit constexpr XYZWrapper(Vector3& position) noexcept
			: XYZWrapper(position.x, position.y, position.z)
		{
		}

		explicit constexpr XYZWrapper(const std::tuple<float&, float&, float&> position) noexcept
			: XYZWrapper(std::get<0>(position), std::get<1>(position), std::get<2>(position))
		{
		}

		constexpr XYZWrapper& operator=(const float(&list)[3]) noexcept
		{
			x = list[0];
			y = list[1];
			z = list[2];
			return *this;
		}

		constexpr XYZWrapper& operator=(const std::span<float, 3> buffer) noexcept
		{
			x = buffer[0];
			y = buffer[1];
			z = buffer[2];
			return *this;
		}

		constexpr XYZWrapper& operator=(const std::span<float> buffer)
		{
			x = buffer[0];
			y = buffer[1];
			z = buffer[2];
			return *this;
		}

		XYZWrapper& operator=(const Vector3& vector) noexcept
		{
			x = vector.x;
			y = vector.y;
			z = vector.z;
			return *this;
		}

		XYZWrapper& operator=(Vector3&& vector) noexcept
		{
			x = std::move(vector.x);
			y = std::move(vector.y);
			z = std::move(vector.z);
			return *this;
		}

		constexpr XYZWrapper& operator=(const XYZWrapper& other) noexcept
		{
			x = other.x;
			y = other.y;
			z = other.z;
			return *this;
		}

		constexpr XYZWrapper& operator=(XYZWrapper&& other) noexcept
		{
			x = std::move(other.x);
			y = std::move(other.y);
			z = std::move(other.z);
			return *this;
		}

		explicit constexpr operator Vector3() const noexcept
		{
			return Vector3{ x, y, z };
		}

		float& x;
		float& y;
		float& z;

	private:
		XYZWrapper(const XYZWrapper&) = delete;
		XYZWrapper(XYZWrapper&&) = delete;
		XYZWrapper(Vector3&& position) = delete;
	};
}
