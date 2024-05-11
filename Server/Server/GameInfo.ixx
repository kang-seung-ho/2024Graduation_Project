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
		void Cleanup() noexcept
		{
			isSummoning = false;
			isAvailable = true;
		}
		
		void Cleanup() volatile noexcept
		{
			isSummoning = false;
			isAvailable = true;
		}

		const int myId;
		float x, y, z;
		std::atomic_bool isAvailable;
		std::atomic_bool isSummoning;
	};

	struct SagaBasePoint
	{
		static inline constexpr float maxHp = 8000;

		constexpr void Cleanup() noexcept
		{
			myHp = maxHp;
		}
		
		constexpr void Cleanup() volatile noexcept
		{
			myHp = maxHp;
		}

		float x, y, z;
		float myHp;
	};

	struct SagaTeamStatus
	{
		constexpr void Cleanup() noexcept
		{
			myScore = 0;

			for (auto& ptr : myMembers)
			{
				ptr = nullptr;
			}
		}

		constexpr void Cleanup() volatile noexcept
		{
			myScore = 0;

			for (auto& ptr : myMembers)
			{
				ptr = nullptr;
			}
		}

		SagaBasePoint myBase;
		int myScore;
		User* myMembers[3];
	};

	namespace SagaGuardianState
	{
		enum Type
		{
			Statue = 0,
			Awakening, Idle, Ridden, Dead
		};
	}

	struct SagaGuardian : public TransformUnit
	{
		static inline constexpr float maxHp = 5000;

		void Update()
		{

		}

		void Cleanup() noexcept
		{
			myStatus = SagaGuardianState::Statue;
			myHp = maxHp;
			riderId = -1;
			isRidingByAnyone = false;
		}
		
		void Cleanup() volatile noexcept
		{
			myStatus = SagaGuardianState::Statue;
			myHp = maxHp;
			riderId = -1;
			isRidingByAnyone = false;
		}

		// Trying to Ride
		[[nodiscard]]
		bool TryRide(std::int32_t user_id) noexcept
		{
			bool was_ridden = isRidingByAnyone.load(std::memory_order_acquire);

			//if (riderId.compare_exchange_strong(-1, user_id))
			{

			}
			//else
			{
				return true;
			}
		}

		[[nodiscard]]
		std::int32_t GetRiderId() const noexcept
		{
			//constexpr std::int64_t maskll = 0x0FFFFFFFFFFFFFFF;
			constexpr std::int64_t maskie = 0x000000007FFFFFFF;

			return static_cast<std::int32_t>(riderInfo.load() & maskie);
		}

		[[nodiscard]]
		bool CanRide() const noexcept
		{
			return myStatus == SagaGuardianState::Idle and not IsRidden();
		}

		[[nodiscard]]
		bool IsActivated() const noexcept
		{
			return myStatus != SagaGuardianState::Statue;
		}

		[[nodiscard]]
		bool IsAlive() const noexcept
		{
			return 0 < myHp and myStatus != SagaGuardianState::Dead;
		}

		[[nodiscard]]
		bool IsDead() const noexcept
		{
			return !IsAlive();
		}

		[[nodiscard]]
		bool IsRidden() const noexcept
		{
			return riderInfo.load() & 0x1000000000000000;
		}

		std::atomic<SagaGuardianState::Type> myStatus;

		/// <summary>
		/// |======= flag ======|
		///  1000 0000 0000 0000
		/// |======= id ========|
		///  0111 1111 1111 1111
		/// </summary>
		std::atomic_int64_t riderInfo;
		std::atomic_int32_t riderId;
		std::atomic_bool isRidingByAnyone;
		float myHp;
	};
}
