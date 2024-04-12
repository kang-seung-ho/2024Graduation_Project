module;
#include <cstddef>
#include <utility>
#include <memory>

export module Iconer.Application.BasicPacket;
export import :Protocol;
export import Iconer.Utility.Serializer;
export import <span>;

namespace iconer::app::packets::detail
{
	template<typename Tuple, size_t... Indices>
	constexpr
		const std::byte*
		Deserialize(const std::byte* buffer, Tuple&& tuple, std::index_sequence<Indices...>)
	{
		((buffer = iconer::util::Deserialize(buffer, std::get<Indices>(tuple))), ...);

		return buffer;
	}
}

namespace iconer::app
{
#pragma pack(push, 1)
	export struct [[nodiscard]] BasicPacket
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
		{}

		explicit constexpr BasicPacket(const PacketProtocol& protocol) noexcept
			: BasicPacket(protocol, SignedMinSize())
		{}

		explicit constexpr BasicPacket(PacketProtocol&& protocol) noexcept
			: BasicPacket(std::move(protocol), SignedMinSize())
		{}

		explicit constexpr BasicPacket(const PacketProtocol& protocol, std::int16_t size) noexcept
			: myProtocol(protocol), mySize(size)
		{}

		explicit constexpr BasicPacket(PacketProtocol&& protocol, std::int16_t size) noexcept
			: myProtocol(std::move(protocol)), mySize(size)
		{}

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

	export namespace packets
	{
		/// <summary>
		/// Create a serialized buffer of empty packet for <paramref name="procotol"/>
		/// <para>-------------------------------------------------------------------------------</para>
		/// <para>throws <see cref="std::bad_alloc"/> when <see cref="iconer::util::Serializes"/> has failed to allocate necessary memory</para>
		/// </summary>
		/// <param name="procotol"/>
		/// <exception cref="std::bad_alloc"/>
		[[nodiscard]]
		constexpr
			std::unique_ptr<std::byte[]>
			Serialize(const PacketProtocol& procotol)
		{
			return iconer::util::Serializes(procotol, static_cast<std::int16_t>(sizeof(PacketProtocol) + sizeof(std::int16_t)));
		}

		template<typename... Args>
			requires (0 < sizeof...(Args))
		[[nodiscard]]
		constexpr
			std::unique_ptr<std::byte[]>
			Serialize(const PacketProtocol& procotol, const std::int16_t& size, Args&&... args)
		{
			auto tuple = std::forward_as_tuple(std::forward<Args>(args)...);
			return iconer::util::Serializes(procotol, iconer::util::GetByteSize(tuple, std::index_sequence_for<Args...>{}), std::move(tuple));
		}

		template<typename... Ts>
		constexpr
			const std::byte*
			Deserialize(const std::byte* buffer, Ts&... outs)
		{
			return detail::Deserialize(buffer, std::tie(outs...), std::index_sequence_for<Ts...>{});
		}
	}
}
