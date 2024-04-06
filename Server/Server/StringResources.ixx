export module Iconer.Application.Resources.String;
import <string>;
import <string_view>;

export namespace iconer::app::resources::string
{
	template<size_t Index>
	struct Table
	{
		static inline constexpr bool IsWideString = false;
	};

#define StoreString(index, text) template<>\
struct iconer::app::resources::string::Table<(index)>\
{\
	static inline constexpr bool IsWideString = false; \
	static inline constexpr auto value = std::basic_string_view{ (text) };\
};

#define StoreWideString(index, text) template<>\
struct iconer::app::resources::string::Table<(index)>\
{\
	static inline constexpr bool IsWideString = true; \
	static inline constexpr auto value = std::basic_string_view{ (L##text) };\
};
}

export StoreWideString(0, "\tWorker {} is started.\n");
export StoreWideString(1, "\tWorker {}: Event by id {}.\n");
export StoreWideString(2, "\tWorker {} is finished\n");
export StoreString(3, "Null packet error.");
export StoreString(4, "Packet size error.");
export StoreString(5, "Unknown packet.");
export StoreString(6, "Error when reserving acceptance of a socket.");
export StoreWideString(7, "\tCannot assembly a packet due to `PacketProcessor`'s failure\n");
export StoreWideString(8, "\tCannot assembly a packet in `PacketProcessor` due to lack of bytes\n");
export StoreWideString(9, "\tA packet is assembled\n");
export StoreWideString(10, "\tCannot assembly a packet due to lack of bytes\n");
export StoreString(11, "This packet is not supported by PacketProcessor\n");
export StoreWideString(12, "\tWorker {} is finished\n");

export namespace iconer::app
{
	inline constexpr size_t StringReourcesCount = 12;

	template<size_t Index>
	[[nodiscard]]
	consteval auto&& GetResourceString() noexcept
	{
		static_assert(Index < StringReourcesCount);

		using StringTable = resources::string::template Table<Index>;
		constexpr auto&& result = StringTable::value;

		return result;
	}
}
