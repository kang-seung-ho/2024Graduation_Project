export module Iconer.App.GameSession;
import <atomic>;

export namespace iconer::app
{
	enum class ESagaItemTypes : std::uint8_t
	{
		Drink = 0,
		Gum,
		SmokeBomb
	};

	struct SagaItem
	{
		ESagaItemTypes myType{};
		std::atomic_bool isAvailable{ true };
		std::atomic_bool isBoxDestroyed{ false };
	};

	enum class ESagaPlayerTeam : std::uint8_t
	{
		Unknown = 0,
		Red, Blu
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

	namespace SagaGuardianState
	{
		enum Type
		{
			Statue = 0,
			Awakening, Idle, Ridden, Dead
		};
	}

	struct SagaGuardian
	{
		static inline constexpr float maxHp = 500;

		std::atomic<SagaGuardianState::Type> myStatus;
		std::atomic_int32_t riderId{ -1 };
		std::atomic<float> myHp{ maxHp };
		std::int32_t myParts[4]{};

		void Update()
		{

		}

		void Cleanup() noexcept
		{
			myStatus = SagaGuardianState::Statue;
			myHp = maxHp;
			riderId = -1;

			for (auto& part : myParts)
			{
				part = 1;
			}
		}

		void Cleanup() volatile noexcept
		{
			myStatus = SagaGuardianState::Statue;
			myHp = maxHp;
			riderId = -1;

			for (auto& part : myParts)
			{
				part = 1;
			}
		}

		// Trying to Ride
		[[nodiscard]]
		bool TryRide(std::int32_t user_id) noexcept
		{
			std::int32_t expected{ -1 };
			if (riderId.compare_exchange_strong(expected, user_id, std::memory_order_acq_rel))
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		bool TryUnride(std::int32_t user_id) noexcept
		{
			if (riderId.compare_exchange_strong(user_id, -1, std::memory_order_acq_rel))
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		[[nodiscard]]
		std::int32_t GetRiderId() const noexcept
		{
			return riderId;
		}

		[[nodiscard]]
		bool IsActivated() const noexcept
		{
			return myStatus != SagaGuardianState::Statue;
		}

		[[nodiscard]]
		bool IsAlive() const noexcept
		{
			//return 0 < myHp and myStatus != SagaGuardianState::Dead;
			return 0 < myHp;
		}

		[[nodiscard]]
		bool IsDead() const noexcept
		{
			return !IsAlive();
		}

		[[nodiscard]]
		bool IsRidden() const noexcept
		{
			return riderId.load() != -1;
		}
	};
}
