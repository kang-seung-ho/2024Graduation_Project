export module Iconer.Application.Room:RoomStates;

export namespace iconer::app
{
	enum class [[nodiscard]] RoomStates
	{
		None
		, Reserved, Creating // first phases: menu scene
		, Idle // lobby scene
		, Closing // room scene
		, Ready // room scene, loading scene
		, Prepared // loading scene
		, InGame // game scene
		, GameDone // game scene
		, Cleanup // game scene, score scene
	};
}
