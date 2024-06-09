export module Iconer.Net.IpAddress;
export import Iconer.Net.IpAddressFamily;
import <type_traits>;
import <string>;
import <string_view>;

export namespace iconer::net
{
	struct [[nodiscard]] IpAddress
	{
		explicit constexpr IpAddress() noexcept = default;
		~IpAddress() noexcept = default;

		explicit constexpr IpAddress(iconer::net::IpAddressFamily family, std::string_view address)
			: addressFamily(family), addressString(address)
		{}

		[[nodiscard]]
		constexpr const iconer::net::IpAddressFamily& GetFamily() const& noexcept
		{
			return addressFamily;
		}

		[[nodiscard]]
		constexpr iconer::net::IpAddressFamily&& GetFamily() && noexcept
		{
			return std::move(addressFamily);
		}

		[[nodiscard]]
		constexpr std::string& GetAddressString() & noexcept
		{
			return addressString;
		}
		
		[[nodiscard]]
		constexpr const std::string& GetAddressString() const& noexcept
		{
			return addressString;
		}

		[[nodiscard]]
		constexpr std::string&& GetAddressString() && noexcept
		{
			return std::move(addressString);
		}

		[[nodiscard]]
		constexpr const std::string&& GetAddressString() const&& noexcept
		{
			return std::move(addressString);
		}

		IpAddress(const IpAddress&) = default;
		IpAddress& operator=(const IpAddress&) = default;
		IpAddress(IpAddress&&) = default;
		IpAddress& operator=(IpAddress&&) = default;
		[[nodiscard]] bool operator==(const IpAddress&) const noexcept = default;

	private:
		iconer::net::IpAddressFamily addressFamily;
		std::string addressString;
	};
}
