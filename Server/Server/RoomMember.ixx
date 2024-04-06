export module Iconer.Application.Room:RoomMember;
import Iconer.Utility.MovableAtomic;

export namespace iconer::app
{
	class [[nodiscard]] User;

	struct [[nodiscard]] RoomMember
	{
		User* myHandle;
		iconer::util::MovableAtomic<bool> isReady;
	};
}
