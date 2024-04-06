export module Iconer.Net.EndPoint;
export import Iconer.Net.IpAddressFamily;
export import Iconer.Net.IpAddress;
import <cstdint>;
import <string>;
import <string_view>;
import <format>;

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

export namespace std
{
	[[nodiscard]]
	string to_string(const iconer::net::EndPoint& ep)
	{
		return std::format("({}) {}:{}", ep.AddressFamily(), ep.IpAddress(), ep.Port());
	}

	[[nodiscard]]
	string to_string(iconer::net::EndPoint&& ep)
	{
		return std::format("({}) {}:{}", std::move(ep).AddressFamily(), std::move(ep).IpAddress(), std::move(ep).Port());
	}
}

export template<>
struct std::formatter<iconer::net::EndPoint, char>
{
	static constexpr format_parse_context::iterator
		parse(format_parse_context& context)
	{
		auto it = context.begin();
		const auto end = context.end();
		if (it == end or *it != '{')
		{
			throw std::format_error{ "Invalid format string." };
		}

		++it;
		if (it != end and *it != '}')
		{
			throw std::format_error{ "Missing '}' in format string." };
		}

		return it;
	}

	static format_context::iterator
		format(const iconer::net::EndPoint& ep, format_context& context)
	{
		return std::format_to(context.out(), "({}) {}:{}", ep.AddressFamily(), ep.IpAddress(), ep.Port());
	}

	static format_context::iterator
		format(iconer::net::EndPoint&& ep, format_context& context)
	{
		return std::format_to(context.out(), "({}) {}:{}", std::move(ep).AddressFamily(), std::move(ep).IpAddress(), std::move(ep).Port());
	}
};
