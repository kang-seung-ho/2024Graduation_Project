export module Iconer.Application.RoomContract;
import Iconer.Utility.Constraints;
import <cstdint>;
import <string>;
import <string_view>;
import <format>;

export namespace iconer::app
{
	enum class [[nodiscard]] RoomContract : std::uint8_t
	{
		Success = 0
		, CannotLocateEmptyRoom // Cannot create a room
		, NoRoomFoundWithId // Cannot find any room with a specific id
		, RoomIsFull // Full of members
		, RoomIsBusy // State mismatching
		, AnotherRoomIsAlreadyAssigned // The client already have a room id
		, NoRoom // The room have not reserved yet
		, InvalidOperation // Room task is invalid
		, UnstableRoom // Room's state is changed at time
		, ServerError // Unknown internal error
	};
}

export namespace std
{
	[[nodiscard]]
	constexpr string to_string(const iconer::app::RoomContract& ctr)
	{
		using enum iconer::app::RoomContract;
		switch (ctr)
		{
			case Success: return "Success";
			case CannotLocateEmptyRoom: return "CannotLocateEmptyRoom";
			case NoRoomFoundWithId: return "NoRoomFoundWithId";
			case RoomIsFull: return "RoomIsFull";
			case RoomIsBusy: return "RoomIsBusy";
			case AnotherRoomIsAlreadyAssigned: return "AnotherRoomIsAlreadyAssigned";
			case NoRoom: return "NoRoom";
			case InvalidOperation: return "InvalidOperation";
			case UnstableRoom: return "UnstableRoom";
			case ServerError: return "ServerError";
			default: return "Unknown";
		}
	}

	[[nodiscard]]
	constexpr wstring to_wstring(const iconer::app::RoomContract& ctr)
	{
		using enum iconer::app::RoomContract;
		switch (ctr)
		{
			case Success: return L"Success";
			case CannotLocateEmptyRoom: return L"CannotLocateEmptyRoom";
			case NoRoomFoundWithId: return L"NoRoomFoundWithId";
			case RoomIsFull: return L"RoomIsFull";
			case RoomIsBusy: return L"RoomIsBusy";
			case AnotherRoomIsAlreadyAssigned: return L"AnotherRoomIsAlreadyAssigned";
			case NoRoom: return L"NoRoom";
			case InvalidOperation: return L"InvalidOperation";
			case UnstableRoom: return L"UnstableRoom";
			case ServerError: return L"ServerError";
			default: return L"Unknown";
		}
	}
}

export template<>
struct std::formatter<iconer::app::RoomContract, char>
{
	static constexpr format_parse_context::iterator
		parse(format_parse_context& context)
	{
		return context.begin();
	}

	static format_context::iterator
		format(const iconer::app::RoomContract& ctr, format_context& context)
	{
		using enum iconer::app::RoomContract;
		switch (ctr)
		{
			case Success: return std::format_to(context.out(), "{}", "Success");
			case CannotLocateEmptyRoom: return std::format_to(context.out(), "{}", "Cannot locate empty room");
			case NoRoomFoundWithId: return std::format_to(context.out(), "{}", "Cannot find any room with a specific id");
			case RoomIsFull: return std::format_to(context.out(), "{}", "Full of members");
			case RoomIsBusy: return std::format_to(context.out(), "{}", "Room mismatching");
			case AnotherRoomIsAlreadyAssigned: return std::format_to(context.out(), "{}", "The client already have a room");
			case NoRoom: return std::format_to(context.out(), "{}", "No room found");
			case InvalidOperation: return std::format_to(context.out(), "{}", "The room task is invalid");
			case UnstableRoom: return std::format_to(context.out(), "{}", "Room is unstable");
			case ServerError: return std::format_to(context.out(), "{}", "Internal server error");
			default: return std::format_to(context.out(), "{}", "Unknown");
		}
	}

	static format_context::iterator
		format(iconer::app::RoomContract&& ctr, format_context& context)
	{
		using enum iconer::app::RoomContract;
		switch (std::move(ctr))
		{
			case Success: return std::format_to(context.out(), "{}", "Success");
			case CannotLocateEmptyRoom: return std::format_to(context.out(), "{}", "Cannot locate empty room");
			case NoRoomFoundWithId: return std::format_to(context.out(), "{}", "Cannot find any room with a specific id");
			case RoomIsFull: return std::format_to(context.out(), "{}", "Full of members");
			case RoomIsBusy: return std::format_to(context.out(), "{}", "Room mismatching");
			case AnotherRoomIsAlreadyAssigned: return std::format_to(context.out(), "{}", "The client already have a room");
			case NoRoom: return std::format_to(context.out(), "{}", "No room found");
			case InvalidOperation: return std::format_to(context.out(), "{}", "The room task is invalid");
			case UnstableRoom: return std::format_to(context.out(), "{}", "Room is unstable");
			case ServerError: return std::format_to(context.out(), "{}", "Internal server error");
			default: return std::format_to(context.out(), "{}", "Unknown");
		}
	}
};

export template<>
struct std::formatter<iconer::app::RoomContract, wchar_t>
{
	static constexpr wformat_parse_context::iterator
		parse(wformat_parse_context& context)
	{
		return context.begin();
	}

	static wformat_context::iterator
		format(const iconer::app::RoomContract& ctr, wformat_context& context)
	{
		using enum iconer::app::RoomContract;
		switch (ctr)
		{
			case Success: return std::format_to(context.out(), L"{}", L"Success");
			case CannotLocateEmptyRoom: return std::format_to(context.out(), L"{}", L"Cannot locate empty room");
			case NoRoomFoundWithId: return std::format_to(context.out(), L"{}", L"Cannot find any room with a specific id");
			case RoomIsFull: return std::format_to(context.out(), L"{}", L"Full of members");
			case RoomIsBusy: return std::format_to(context.out(), L"{}", L"Room mismatching");
			case AnotherRoomIsAlreadyAssigned: return std::format_to(context.out(), L"{}", L"The client already have a room");
			case NoRoom: return std::format_to(context.out(), L"{}", L"No room found");
			case InvalidOperation: return std::format_to(context.out(), L"{}", L"The room task is invalid");
			case UnstableRoom: return std::format_to(context.out(), L"{}", L"Room is unstable");
			case ServerError: return std::format_to(context.out(), L"{}", L"Internal server error");
			default: return std::format_to(context.out(), L"{}", L"Unknown");
		}
	}

	static wformat_context::iterator
		format(iconer::app::RoomContract&& ctr, wformat_context& context)
	{
		using enum iconer::app::RoomContract;
		switch (std::move(ctr))
		{
			case Success: return std::format_to(context.out(), L"{}", L"Success");
			case CannotLocateEmptyRoom: return std::format_to(context.out(), L"{}", L"Cannot locate empty room");
			case NoRoomFoundWithId: return std::format_to(context.out(), L"{}", L"Cannot find any room with a specific id");
			case RoomIsFull: return std::format_to(context.out(), L"{}", L"Full of members");
			case RoomIsBusy: return std::format_to(context.out(), L"{}", L"Room mismatching");
			case AnotherRoomIsAlreadyAssigned: return std::format_to(context.out(), L"{}", L"The client already have a room");
			case NoRoom: return std::format_to(context.out(), L"{}", L"No room found");
			case InvalidOperation: return std::format_to(context.out(), L"{}", L"The room task is invalid");
			case UnstableRoom: return std::format_to(context.out(), L"{}", L"Room is unstable");
			case ServerError: return std::format_to(context.out(), L"{}", L"Internal server error");
			default: return std::format_to(context.out(), L"{}", L"Unknown");
		}
	}
};
