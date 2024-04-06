export module Iconer.Application.GameContract;
import Iconer.Utility.Constraints;
import <cstdint>;
import <string>;
import <string_view>;
import <format>;

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

export template<>
struct std::formatter<iconer::app::GameContract, char>
{
	static constexpr format_parse_context::iterator
		parse(format_parse_context& context)
	{
		return context.begin();
	}

	static format_context::iterator
		format(iconer::app::GameContract ctr, format_context& context)
	{
		using enum iconer::app::GameContract;
		switch (ctr)
		{
			case Unknown: return std::format_to(context.out(), "{}", "Unknown");
			case NotInRoom: return std::format_to(context.out(), "{}", "The client is not in a room");
			case ClientIsBusy: return std::format_to(context.out(), "{}", "The client's state is unavailable");
			case LackOfMember: return std::format_to(context.out(), "{}", "The room's number of member is lack of starting a game");
			case InvalidRoom: return std::format_to(context.out(), "{}", "The room is unavailable");
			case InvalidOperation: return std::format_to(context.out(), "{}", "The start task is invalid");
			case UnstableRoom: return std::format_to(context.out(), "{}", "Room is unstable");
			case ServerError: return std::format_to(context.out(), "{}", "Internal server error");
			default: return std::format_to(context.out(), "{}", "Unknown");
		}
	}
};

export template<>
struct std::formatter<iconer::app::GameContract, wchar_t>
{
	static constexpr wformat_parse_context::iterator
		parse(wformat_parse_context& context)
	{
		return context.begin();
	}

	static wformat_context::iterator
		format(iconer::app::GameContract ctr, wformat_context& context)
	{
		using enum iconer::app::GameContract;
		switch (ctr)
		{
			case Unknown: return std::format_to(context.out(), L"{}", L"Unknown");
			case NotInRoom: return std::format_to(context.out(), L"{}", L"The client is not in a room");
			case ClientIsBusy: return std::format_to(context.out(), L"{}", L"The client's state is unavailable");
			case LackOfMember: return std::format_to(context.out(), L"{}", L"The room's number of member is lack of starting a game");
			case InvalidRoom: return std::format_to(context.out(), L"{}", L"The room is unavailable");
			case InvalidOperation: return std::format_to(context.out(), L"{}", L"The start task is invalid");
			case UnstableRoom: return std::format_to(context.out(), L"{}", L"Room is unstable");
			case ServerError: return std::format_to(context.out(), L"{}", L"Internal server error");
			default: return std::format_to(context.out(), L"{}", L"Unknown");
		}
	}
};
