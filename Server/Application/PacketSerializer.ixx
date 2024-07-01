export module Iconer.App.PacketSerializer;
import Iconer.Utility.TypeTraits;
import iconer.Utility.Serializer;
import Iconer.App.PacketProtocol;
import <memory>;

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

	template<typename... Ts>
	[[nodiscard]]
	constexpr std::unique_ptr<std::byte[]> Serialize(const PacketProtocol protocol, Ts&&... params)
	{
		constexpr std::int16_t packet_default_size = sizeof(PacketProtocol) + sizeof(std::int16_t);

		auto result = std::make_unique<std::byte[]>(packet_default_size);

		(void)SerializeAt(result.get(), protocol, std::forward<Ts>(params)...);

		return std::move(result);
	}
}
