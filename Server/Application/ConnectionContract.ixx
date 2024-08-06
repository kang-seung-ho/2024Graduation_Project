export module Iconer.App.ConnectionContract;
import <cstdint>;

export namespace iconer::app
{
	enum class [[nodiscard]] ConnectionContract : std::uint32_t
	{
		Success = 0
		, NoSocket
		, SignInFailed
		, CannotCreateWorker
		, ConnectError
		, WrongAddress
		, OtherError
	};
}