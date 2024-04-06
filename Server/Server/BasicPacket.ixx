module;
#include <cstddef>
#include <utility>

export module Iconer.Application.BasicPacket;
export import :Protocol;
export import Iconer.Utility.Serializer;
export import <span>;

export namespace iconer::app
{
#pragma pack(push, 1)
	struct [[nodiscard]] BasicPacket
	{
		[[nodiscard]]
		static consteval size_t MinSize() noexcept
		{
			return sizeof(PacketProtocol) + sizeof(std::int16_t);
		}

		[[nodiscard]]
		static consteval ptrdiff_t SignedMinSize() noexcept
		{
			return static_cast<ptrdiff_t>(sizeof(PacketProtocol) + sizeof(std::int16_t));
		}

		constexpr BasicPacket() noexcept
			: myProtocol(), mySize(0)
		{
		}

		explicit constexpr BasicPacket(const PacketProtocol& protocol) noexcept
			: BasicPacket(protocol, SignedMinSize())
		{
		}

		explicit constexpr BasicPacket(PacketProtocol&& protocol) noexcept
			: BasicPacket(std::move(protocol), SignedMinSize())
		{
		}

		explicit constexpr BasicPacket(const PacketProtocol& protocol, std::int16_t size) noexcept
			: myProtocol(protocol), mySize(size)
		{
		}

		explicit constexpr BasicPacket(PacketProtocol&& protocol, std::int16_t size) noexcept
			: myProtocol(std::move(protocol)), mySize(size)
		{
		}

		constexpr std::byte* Write(std::byte* buffer) const
		{
			return iconer::util::Serialize(iconer::util::Serialize(buffer, std::to_underlying(myProtocol)), mySize);
		}

		constexpr const std::byte* Read(const std::byte* buffer)
		{
			return iconer::util::Deserialize(iconer::util::Deserialize(buffer, myProtocol), mySize);
		}

		PacketProtocol myProtocol;
		std::int16_t mySize;
	};
#pragma pack(pop)
}
