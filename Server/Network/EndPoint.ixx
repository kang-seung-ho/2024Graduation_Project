export module Iconer.Net.EndPoint;
export import Iconer.Net.IpAddressFamily;
export import Iconer.Net.IpAddress;
import <cstdint>;
import <string_view>;

export namespace iconer::net
{
	struct [[nodiscard]] EndPoint
	{
		explicit EndPoint() = default;

		explicit constexpr EndPoint(const iconer::net::IpAddress& ip_address, std::uint16_t port)
			: myAddress(ip_address), myPort(port)
		{}

		explicit constexpr EndPoint(iconer::net::IpAddress&& ip_address, std::uint16_t port)
			: myAddress(std::move(ip_address)), myPort(port)
		{}

		[[nodiscard]]
		constexpr const iconer::net::IpAddressFamily& GetAddressFamily() const& noexcept
		{
			return myAddress.GetFamily();
		}

		[[nodiscard]]
		constexpr iconer::net::IpAddressFamily&& GetAddressFamily() && noexcept
		{
			return std::move(myAddress).GetFamily();
		}

		[[nodiscard]]
		constexpr const iconer::net::IpAddress& GetIpAddress() const& noexcept
		{
			return myAddress;
		}

		[[nodiscard]]
		constexpr iconer::net::IpAddress&& GetIpAddress() && noexcept
		{
			return std::move(myAddress);
		}

		[[nodiscard]]
		constexpr const std::uint16_t& GetPort() const& noexcept
		{
			return myPort;
		}

		[[nodiscard]]
		constexpr std::uint16_t&& GetPort() && noexcept
		{
			return std::move(myPort);
		}

		[[nodiscard]]
		std::string_view GetAddressString() const noexcept
		{
			return myAddress.GetAddressString();
		}

		EndPoint(const EndPoint&) = default;
		EndPoint& operator=(const EndPoint&) = default;
		EndPoint(EndPoint&&) = default;
		EndPoint& operator=(EndPoint&&) = default;
		[[nodiscard]] constexpr bool operator==(const EndPoint&) const noexcept = default;

	private:
		iconer::net::IpAddress myAddress;
		std::uint16_t myPort;
	};
}
