export module Iconer.Utility.InternalObject;

export namespace iconer::util
{
	class InternalObject
	{
	public:
		constexpr InternalObject() noexcept = default;
		constexpr ~InternalObject() noexcept = default;

		constexpr InternalObject(const InternalObject&) noexcept
		{}

		constexpr InternalObject(InternalObject&&) noexcept
		{}

		constexpr InternalObject& operator=(const InternalObject&) noexcept
		{
			return *this;
		}

		constexpr InternalObject& operator=(InternalObject&&) noexcept
		{
			return *this;
		}
	};
}
