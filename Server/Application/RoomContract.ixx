module;
#include <cstdint>
#include <string>
#include <string_view>
export module Iconer.App.RoomContract;

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
