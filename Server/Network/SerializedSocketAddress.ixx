export module Iconer.Net.SerializedSocketAddress;
import Iconer.Net.IpAddressFamily;

export namespace iconer::net
{
	struct [[nodiscard]] SerializedSocketAddress
	{
		constexpr SerializedSocketAddress() noexcept = default;
		constexpr ~SerializedSocketAddress() noexcept = default;

		IpAddressFamily myFamily;
		char myData[128 - sizeof(IpAddressFamily)];
	};
}
