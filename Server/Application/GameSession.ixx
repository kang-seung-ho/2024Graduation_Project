export module Iconer.App.GameSession;
import <chrono>;
import <array>;
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
			Idle = 0,
			Dead
		};
	}

	struct SagaSmallGummyBear
	{
		static inline constexpr float maxHp = 100;

		std::atomic_bool isMorphed{};
		std::atomic_bool isActivated{};
		std::atomic<float> myHp{ maxHp };
		float x{}, y{}, z{}, yaw{};
		std::chrono::system_clock::time_point activeTime;

		bool TryMorph() volatile noexcept
		{
			bool active = false;

			return isMorphed.compare_exchange_strong(active, true);
		}

		bool TryCancelMorph() volatile noexcept
		{
			bool active = true;

			return isMorphed.compare_exchange_strong(active, false);
		}

		bool TryActivate() volatile noexcept
		{
			bool active = false;

			return isActivated.compare_exchange_strong(active, true);
		}

		bool TryDeactivate() volatile noexcept
		{
			bool active = true;

			return isActivated.compare_exchange_strong(active, false);
		}
	};

	struct SagaGuardian
	{
		static inline constexpr float maxHp = 500;

		std::atomic<SagaGuardianState::Type> myStatus;
		std::atomic<float> myHp{ maxHp };
		std::atomic_int32_t riderId{ -1 };

		SagaSmallGummyBear myPartEntities[4]{};
		std::int32_t myPartHealthPoints[4]{ 1, 1, 1, 1 };

		SagaSmallGummyBear& GetPartedEntity(int index)
		{
			return myPartEntities[index];
		}

		void Cleanup() noexcept
		{
			myStatus = SagaGuardianState::Idle;
			myHp = maxHp;
			riderId = -1;

			for (auto& entity : myPartEntities)
			{
				entity.isMorphed = false;
				entity.isActivated = false;
				entity.myHp = entity.maxHp;
			}

			for (auto& part : myPartHealthPoints)
			{
				part = 1;
			}
		}

		void Cleanup() volatile noexcept
		{
			myStatus = SagaGuardianState::Idle;
			myHp = maxHp;
			riderId = -1;

			for (auto& entity : myPartEntities)
			{
				entity.isActivated = false;
				entity.myHp = entity.maxHp;
			}

			for (auto& part : myPartHealthPoints)
			{
				part = 1;
			}
		}

		// Trying to Ride
		[[nodiscard]]
		bool TryRide(std::int32_t user_id) volatile noexcept
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

		bool TryUnride(std::int32_t user_id) volatile noexcept
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
		std::int32_t GetRiderId() const volatile noexcept
		{
			return riderId.load(std::memory_order_acquire);
		}

		[[nodiscard]]
		bool IsAlive() const volatile noexcept
		{
			//return 0 < myHp and myStatus != SagaGuardianState::Dead;
			return 0 < myHp.load(std::memory_order_acquire);
		}

		[[nodiscard]]
		bool IsDead() const volatile noexcept
		{
			return !IsAlive();
		}

		[[nodiscard]]
		bool IsRidden() const volatile noexcept
		{
			return riderId.load(std::memory_order_acquire) != -1;
		}
	};
}
