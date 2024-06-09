export module Iconer.Net.EndPoint;
export import Iconer.Net.IpAddressFamily;
export import Iconer.Net.IpAddress;
import <cstdint>;
import <string_view>;

export namespace iconer::net
{
	struct [[nodiscard]] IpAddress;

	struct [[nodiscard]] EndPoint
	{
		explicit EndPoint() = default;
		~EndPoint() = default;

		explicit constexpr EndPoint(const IpAddress& ip_address, std::uint16_t port)
			: myAddress(ip_address), myPort(port)
		{}

		explicit constexpr EndPoint(IpAddress&& ip_address, std::uint16_t port)
			: myAddress(std::move(ip_address)), myPort(port)
		{}

		[[nodiscard]]
		constexpr const IpAddressFamily& AddressFamily() const& noexcept
		{
			return myAddress.GetFamily();
		}

		[[nodiscard]]
		constexpr IpAddressFamily&& AddressFamily() && noexcept
		{
			return std::move(myAddress).GetFamily();
		}

		[[nodiscard]]
		constexpr const iconer::net::IpAddress& IpAddress() const& noexcept
		{
			return myAddress;
		}

		[[nodiscard]]
		constexpr iconer::net::IpAddress&& IpAddress() && noexcept
		{
			return std::move(myAddress);
		}

		[[nodiscard]]
		constexpr const std::uint16_t& Port() const& noexcept
		{
			return myPort;
		}

		[[nodiscard]]
		constexpr std::uint16_t&& Port() && noexcept
		{
			return std::move(myPort);
		}

		[[nodiscard]]
		std::string_view GetAddressString() const noexcept
		{
			return myAddress.GetAddressString();
		}

		[[nodiscard]] constexpr bool operator==(const EndPoint&) const noexcept = default;

		iconer::net::IpAddress myAddress;
		std::uint16_t myPort;
	};
}
