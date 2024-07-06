module;
#include <utility>
#include <memory>

export module Iconer.App.PacketSerializer;
import Iconer.Utility.TypeTraits;
import iconer.Utility.Serializer;
import Iconer.App.PacketProtocol;
import <cstddef>;
import <array>;

export namespace iconer::app
{
	template<typename... Ts>
	constexpr std::byte* SerializeAt(std::byte* buffer, const PacketProtocol protocol, Ts&&... params)
	{
		constexpr std::int16_t packet_default_size = sizeof(PacketProtocol) + sizeof(std::int16_t);
		const std::int16_t packet_size = packet_default_size + iconer::util::GetTotalByteSize(params...);

		auto seek = iconer::util::Serialize(buffer, protocol);

		seek = iconer::util::Serialize(seek, packet_size);

		//constexpr int vals[] = { 0, 1, 2, 3 };
		//seek = iconer::util::Serialize(seek, vals);
		//seek = iconer::util::Serialize(seek, { 0, 1, 2, 3 });
		//seek = iconer::util::Serialize(seek, L"fdfdf");

		((seek = iconer::util::Serialize(seek, std::forward<Ts>(params))), ...);

		return static_cast<std::byte*>(seek);
	}

	template<size_t Len, typename... Ts>
	constexpr std::byte* SerializeAt(std::array<std::byte, Len>& buffer, const PacketProtocol protocol, Ts&&... params)
	{
		return SerializeAt(buffer.data(), protocol, std::forward<Ts>(params)...);
	}

	template<typename... Ts>
	[[nodiscard]]
	constexpr std::pair<std::unique_ptr<std::byte[]>, std::int16_t> Serialize(const PacketProtocol protocol, Ts&&... params)
	{
		const std::int16_t packet_size = sizeof(PacketProtocol) + sizeof(std::int16_t) + iconer::util::GetTotalByteSize(params...);

		auto result = std::make_unique<std::byte[]>(packet_size);

		(void)SerializeAt(result.get(), protocol, std::forward<Ts>(params)...);

		return std::make_pair(std::move(result), packet_size);
	}
}
