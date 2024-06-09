export module Iconer.Application.TransformUnit;
import <type_traits>;
import <tuple>;

export namespace iconer::app
{
	struct TransformUnit
	{
		constexpr TransformUnit& PositionX(const float& v) noexcept
		{
			myX = v;
			return *this;
		}

		constexpr TransformUnit& PositionX(float&& v) noexcept
		{
			myX = std::move(v);
			return *this;
		}

		constexpr TransformUnit& PositionY(const float& v) noexcept
		{
			myY = v;
			return *this;
		}

		constexpr TransformUnit& PositionY(float&& v) noexcept
		{
			myY = std::move(v);
			return *this;
		}

		constexpr TransformUnit& PositionZ(const float& v) noexcept
		{
			myZ = v;
			return *this;
		}
		
		constexpr TransformUnit& PositionZ(float&& v) noexcept
		{
			myZ = std::move(v);
			return *this;
		}

		constexpr TransformUnit& Positions(const float& x, const float& y, const float& z) noexcept
		{
			myX = x;
			myY = y;
			myZ = z;
			return *this;
		}
		
		constexpr TransformUnit& Positions(float&& x, const float& y, const float& z) noexcept
		{
			myX = std::move(x);
			myY = y;
			myZ = z;
			return *this;
		}

		constexpr TransformUnit& Positions(const float& x, float&& y, const float& z) noexcept
		{
			myX = x;
			myY = std::move(y);
			myZ = z;
			return *this;
		}

		constexpr TransformUnit& Positions(const float& x, const float&& y, float&& z) noexcept
		{
			myX = x;
			myY = y;
			myZ = std::move(z);
			return *this;
		}
		
		constexpr TransformUnit& Positions(float&& x, float&& y, const float& z) noexcept
		{
			myX = std::move(x);
			myY = std::move(y);
			myZ = z;
			return *this;
		}

		constexpr TransformUnit& Positions(const float& x, float&& y, float&& z) noexcept
		{
			myX = x;
			myY = std::move(y);
			myZ = std::move(z);
			return *this;
		}

		constexpr TransformUnit& Positions(float&& x, const float&& y, float&& z) noexcept
		{
			myX = std::move(x);
			myY = y;
			myZ = std::move(z);
			return *this;
		}

		constexpr TransformUnit& Positions(float&& x, float&& y, float&& z) noexcept
		{
			myX = std::move(x);
			myY = std::move(y);
			myZ = std::move(z);
			return *this;
		}

		[[nodiscard]]
		constexpr const float& PositionX() const& noexcept
		{
			return myX;
		}

		[[nodiscard]]
		constexpr const float& PositionY() const& noexcept
		{
			return myY;
		}

		[[nodiscard]]
		constexpr const float& PositionZ() const& noexcept
		{
			return myZ;
		}
		
		[[nodiscard]]
		constexpr float&& PositionX() && noexcept
		{
			return std::move(myX);
		}

		[[nodiscard]]
		constexpr float&& PositionY() && noexcept
		{
			return std::move(myY);
		}

		[[nodiscard]]
		constexpr float&& PositionZ() && noexcept
		{
			return std::move(myZ);
		}

		[[nodiscard]]
		constexpr auto Positions() const& noexcept
		{
			return std::tie(myX, myY, myZ);
		}

		[[nodiscard]]
		constexpr auto Positions() && noexcept
		{
			return std::forward_as_tuple(std::move(myX), std::move(myY), std::move(myZ));
		}

		constexpr TransformUnit& Rotations(const float& r, const float& y, const float& p) noexcept
		{
			myRoll = r;
			myYaw = y;
			myPitch = p;
			return *this;
		}

		constexpr TransformUnit& RotationLook(const float& v) noexcept
		{
			myPitch = v;
			return *this;
		}

		constexpr TransformUnit& RotationLook(float&& v) noexcept
		{
			myPitch = std::move(v);
			return *this;
		}

		constexpr TransformUnit& RotationUp(const float& v) noexcept
		{
			myYaw = v;
			return *this;
		}

		constexpr TransformUnit& RotationUp(float&& v) noexcept
		{
			myYaw = std::move(v);
			return *this;
		}

		constexpr TransformUnit& RotationRight(const float& v) noexcept
		{
			myRoll = v;
			return *this;
		}
		
		constexpr TransformUnit& RotationRight(float&& v) noexcept
		{
			myRoll = std::move(v);
			return *this;
		}

		[[nodiscard]]
		constexpr const float& RotationLook() const& noexcept
		{
			return myPitch;
		}

		[[nodiscard]]
		constexpr const float& RotationUp() const& noexcept
		{
			return myYaw;
		}

		[[nodiscard]]
		constexpr const float& RotationRight() const& noexcept
		{
			return myRoll;
		}
		
		[[nodiscard]]
		constexpr float&& RotationLook() && noexcept
		{
			return std::move(myPitch);
		}

		[[nodiscard]]
		constexpr float&& RotationUp() && noexcept
		{
			return std::move(myYaw);
		}

		[[nodiscard]]
		constexpr float&& RotationRight() && noexcept
		{
			return std::move(myRoll);
		}

		float myX, myY, myZ, myRoll, myYaw, myPitch;
	};
}
