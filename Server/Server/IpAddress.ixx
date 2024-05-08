export module Iconer.Net.IpAddress;
import Iconer.Utility.Constraints;
export import Iconer.Net.IpAddressFamily;
import <utility>;
import <string>;
import <string_view>;

export namespace iconer::net
{
	struct [[nodiscard]] IpAddress
	{
		explicit constexpr IpAddress() noexcept(nothrow_default_constructibles<std::string>) = default;
		~IpAddress() noexcept(nothrow_destructibles<std::string>) = default;
		
		constexpr IpAddress(const IpAddress&) noexcept(nothrow_copy_constructibles<std::string>) = default;
		constexpr IpAddress& operator=(const IpAddress&) noexcept(nothrow_copy_assignables<std::string>) = default;
		constexpr IpAddress(IpAddress&&) noexcept(nothrow_move_constructibles<std::string>) = default;
		constexpr IpAddress& operator=(IpAddress&&) noexcept(nothrow_move_assignables<std::string>) = default;

		explicit constexpr IpAddress(IpAddressFamily family, std::string_view address)
			: addressFamily(family), addressString(address)
		{}

		[[nodiscard]]
		constexpr const IpAddressFamily& GetFamily() const& noexcept
		{
			return addressFamily;
		}

		[[nodiscard]]
		constexpr IpAddressFamily&& GetFamily() && noexcept
		{
			return std::move(addressFamily);
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

		[[nodiscard]]
		constexpr bool operator==(const IpAddress&) const noexcept = default;

		IpAddressFamily addressFamily;
		std::string addressString;
	};
}
