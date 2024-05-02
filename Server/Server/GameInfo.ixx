module;
#include <atomic>

export module Iconer.Application.GameInfo;
import Iconer.Application.TransformUnit;

export namespace iconer::app
{
	class User;

	struct SagaSummonPoint
	{
		const int myId;
		float x, y, z;
		std::atomic_bool isAvailable;
		std::atomic_bool isSummoning;
	};

	struct SagaBasePoint
	{
		float x, y, z;
		float myHp;
	};

	struct SagaTeamStatus
	{
		SagaBasePoint myBase;
		int myScore;
		User* myMembers[3];
	};

	struct SagaGuardian : public TransformUnit
	{
		std::atomic_bool isActivated;
		std::atomic_bool isAlive;
		std::atomic_bool isRidingAnyone;
		float myHp;
	};
}
