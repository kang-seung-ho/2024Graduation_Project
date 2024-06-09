module;
#include <cstdint>
#include <string>
#include <string_view>

export module Iconer.Application.GameContract;
import Iconer.Utility.Constraints;

export namespace iconer::app
{
	enum class [[nodiscard]] GameContract : std::uint8_t
	{
		Unknown = 0
		, NotInRoom
		, ClientIsBusy // The client's state is unmatched
		, LackOfMember
		, InvalidRoom
		, InvalidOperation // Room task is invalid
		, UnstableRoom // Room's state is changed at time
		, OtherClientFailed // Other member(s) in the room could not prepare the game
		, ServerError // Unknown internal error
		, Success = 200
	};
}

export namespace std
{
	[[nodiscard]]
	constexpr string to_string(const iconer::app::GameContract& ctr)
	{
		using enum iconer::app::GameContract;
		switch (ctr)
		{
			case Unknown: return "Unknown";
			case NotInRoom: return "NotInRoom";
			case ClientIsBusy: return "ClientIsBusy";
			case LackOfMember: return "LackOfMember";
			case InvalidRoom: return "InvalidRoom";
			case InvalidOperation: return "InvalidOperation";
			case UnstableRoom: return "UnstableRoom";
			case ServerError: return "ServerError";
			default: return "Unknown";
		}
	}

	[[nodiscard]]
	constexpr wstring to_wstring(const iconer::app::GameContract& ctr)
	{
		using enum iconer::app::GameContract;
		switch (ctr)
		{
			case Unknown: return L"Unknown";
			case NotInRoom: return L"NotInRoom";
			case ClientIsBusy: return L"ClientIsBusy";
			case LackOfMember: return L"LackOfMember";
			case InvalidRoom: return L"InvalidRoom";
			case InvalidOperation: return L"InvalidOperation";
			case UnstableRoom: return L"UnstableRoom";
			case ServerError: return L"ServerError";
			default: return L"Unknown";
		}
	}
}

/*
template<>
struct std::formatter<iconer::app::RoomContract, char>
{
	constexpr format_parse_context::iterator parse(format_parse_context& context) const
	{
		return context.begin();
	}

	format_context::iterator format(const iconer::app::RoomContract& ctr, format_context& context) const
	{
		using namespace std::string_view_literals;
		using enum iconer::app::RoomContract;

		switch (ctr)
		{
		case Success: return std::format_to(context.out(), "{}"sv, "Success");
		case CannotLocateEmptyRoom: return std::format_to(context.out(), "{}"sv, "Cannot locate empty room");
		case NoRoomFoundWithId: return std::format_to(context.out(), "{}"sv, "Cannot find any room with a specific id");
		case RoomIsFull: return std::format_to(context.out(), "{}"sv, "Full of members");
		case RoomIsBusy: return std::format_to(context.out(), "{}"sv, "Room mismatching");
		case AnotherRoomIsAlreadyAssigned: return std::format_to(context.out(), "{}"sv, "The client already have a room");
		case NoRoom: return std::format_to(context.out(), "{}"sv, "No room found");
		case InvalidOperation: return std::format_to(context.out(), "{}"sv, "The room task is invalid");
		case UnstableRoom: return std::format_to(context.out(), "{}"sv, "Room is unstable");
		case ServerError: return std::format_to(context.out(), "{}"sv, "Internal server error");
		default: return std::format_to(context.out(), "{}"sv, "Unknown");
		}
	}
};

template<>
struct std::formatter<iconer::app::RoomContract, wchar_t>
{
	constexpr wformat_parse_context::iterator parse(wformat_parse_context& context) const
	{
		return context.begin();
	}

	wformat_context::iterator format(const iconer::app::RoomContract& ctr, wformat_context& context) const
	{
		using namespace std::string_view_literals;
		using enum iconer::app::RoomContract;

		switch (std::move(ctr))
		{
		case Success: return std::format_to(context.out(), L"{}"sv, L"Success");
		case CannotLocateEmptyRoom: return std::format_to(context.out(), L"{}"sv, L"Cannot locate empty room");
		case NoRoomFoundWithId: return std::format_to(context.out(), L"{}"sv, L"Cannot find any room with a specific id");
		case RoomIsFull: return std::format_to(context.out(), L"{}"sv, L"Full of members");
		case RoomIsBusy: return std::format_to(context.out(), L"{}"sv, L"Room mismatching");
		case AnotherRoomIsAlreadyAssigned: return std::format_to(context.out(), L"{}"sv, L"The client already have a room");
		case NoRoom: return std::format_to(context.out(), L"{}"sv, L"No room found");
		case InvalidOperation: return std::format_to(context.out(), L"{}"sv, L"The room task is invalid");
		case UnstableRoom: return std::format_to(context.out(), L"{}"sv, L"Room is unstable");
		case ServerError: return std::format_to(context.out(), L"{}"sv, L"Internal server error");
		default: return std::format_to(context.out(), L"{}"sv, L"Unknown");
		}
	}
};
*/

/*
template<>
struct std::formatter<iconer::app::GameContract, char>
{
	constexpr format_parse_context::iterator parse(format_parse_context& context) const
	{
		return context.begin();
	}

	format_context::iterator format(iconer::app::GameContract ctr, format_context& context) const
	{
		using namespace std::string_view_literals;
		using enum iconer::app::GameContract;

		switch (ctr)
		{
		case Unknown: return std::format_to(context.out(), "{}"sv, "Unknown");
		case NotInRoom: return std::format_to(context.out(), "{}"sv, "The client is not in a room");
		case ClientIsBusy: return std::format_to(context.out(), "{}"sv, "The client's state is unavailable");
		case LackOfMember: return std::format_to(context.out(), "{}"sv, "The room's number of member is lack of starting a game");
		case InvalidRoom: return std::format_to(context.out(), "{}"sv, "The room is unavailable");
		case InvalidOperation: return std::format_to(context.out(), "{}"sv, "The start task is invalid");
		case UnstableRoom: return std::format_to(context.out(), "{}"sv, "Room is unstable");
		case ServerError: return std::format_to(context.out(), "{}"sv, "Internal server error");
		default: return std::format_to(context.out(), "{}"sv, "Unknown");
		}
	}
};

template<>
struct std::formatter<iconer::app::GameContract, wchar_t>
{
	constexpr wformat_parse_context::iterator parse(wformat_parse_context& context) const
	{
		return context.begin();
	}

	wformat_context::iterator format(iconer::app::GameContract ctr, wformat_context& context) const
	{
		using namespace std::string_view_literals;
		using enum iconer::app::GameContract;

		switch (ctr)
		{
		case Unknown: return std::format_to(context.out(), L"{}"sv, L"Unknown");
		case NotInRoom: return std::format_to(context.out(), L"{}"sv, L"The client is not in a room");
		case ClientIsBusy: return std::format_to(context.out(), L"{}"sv, L"The client's state is unavailable");
		case LackOfMember: return std::format_to(context.out(), L"{}"sv, L"The room's number of member is lack of starting a game");
		case InvalidRoom: return std::format_to(context.out(), L"{}"sv, L"The room is unavailable");
		case InvalidOperation: return std::format_to(context.out(), L"{}"sv, L"The start task is invalid");
		case UnstableRoom: return std::format_to(context.out(), L"{}"sv, L"Room is unstable");
		case ServerError: return std::format_to(context.out(), L"{}"sv, L"Internal server error");
		default: return std::format_to(context.out(), L"{}"sv, L"Unknown");
		}
	}
};
*/
