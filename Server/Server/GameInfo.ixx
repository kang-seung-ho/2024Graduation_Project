module;
#include <atomic>

export module Iconer.Application.GameInfo;
import Iconer.Application.TransformUnit;

namespace iconer::app
{
	class User;
}

export namespace iconer::app::game
{
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

	namespace SagaGuardianState
	{
		enum Type
		{
			Statue = 0,
			Awakening, Idle, Dead
		};
	}

	struct SagaGuardian : public TransformUnit
	{
		SagaGuardianState::Type myStatus;

		[[nodiscard]]
		constexpr bool IsActivated() const noexcept
		{
			return myStatus != SagaGuardianState::Statue;
		}

		[[nodiscard]]
		constexpr bool IsAlive() const noexcept
		{
			return myStatus != SagaGuardianState::Dead;
		}

		[[nodiscard]]
		constexpr bool IsDead() const noexcept
		{
			return myStatus == SagaGuardianState::Dead;
		}

		std::atomic_bool isRidingByAnyone;
		float myHp;
	};
}
