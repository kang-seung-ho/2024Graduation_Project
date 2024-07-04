module;
#include <mutex>

export module Iconer.App.Room;
import Iconer.Utility.ReadOnly;
import Iconer.Utility.Delegate;
import Iconer.Utility.Container.AtomicQueue;
import Iconer.Net.ErrorCode;
import Iconer.Net.Socket;
import Iconer.App.ISession;
import Iconer.App.TaskContext;
import Iconer.App.Settings;
import <array>;
import <atomic>;

export namespace iconer::app
{
	class [[nodiscard]] User;

	class [[nodiscard]] Member
	{
	private:
		std::atomic<iconer::app::User*> userHandle{};

	public:
		Member() noexcept = default;

		[[nodiscard]]
		bool TryJoin(iconer::app::User* user) noexcept
		{
			iconer::app::User* null = nullptr;
			return userHandle.compare_exchange_strong(null, user);
		}

		[[nodiscard]]
		bool ChangedToEmpty(iconer::app::User* before_user, std::memory_order order = std::memory_order_release) noexcept
		{
			return userHandle.compare_exchange_strong(before_user, nullptr, order);
		}

		friend class Room;
	};

	class [[nodiscard]] Room : public ISession
	{
	public:
		using super = ISession;
		using this_class = Room;
		using id_type = super::id_type;

		static inline constexpr size_t userLimits = Settings::roomMembersLimit;

		iconer::util::Delegate<void, iconer::app::Room*, iconer::app::User*> onOccupied{};
		iconer::util::Delegate<void, iconer::app::Room*> onDestroyed{};
		iconer::util::Delegate<void, iconer::app::Room*, iconer::app::User*, std::int32_t> onUserJoined{};
		iconer::util::Delegate<void, iconer::app::Room*, iconer::app::User*, std::int32_t> onUserLeft{};
		iconer::util::Delegate<void, iconer::app::Room*, iconer::app::User*> onFailedToOccupy{};
		iconer::util::Delegate<void, iconer::app::Room*, iconer::app::User*> onFailedToJoinUser{};

		Room() = default;
		~Room() = default;

		[[nodiscard]]
		bool TryOccupy(iconer::app::User& user);

		[[nodiscard]]
		bool TryJoin(iconer::app::User& user);

		bool Leave(iconer::app::User& user) noexcept;

		[[nodiscard]]
		bool IsTaken() const noexcept
		{
			return isTaken.load(std::memory_order_acquire);
		}

	private:
		std::atomic_bool isTaken{};
		std::array<Member, userLimits> myMembers{};
		std::atomic_int32_t memberCount{};
		std::mutex memberRemoverLock{};

		[[nodiscard]]
		bool ChangedIsTaken(bool before, bool flag, std::memory_order order = std::memory_order_release) noexcept
		{
			return isTaken.compare_exchange_strong(before, flag, order);
		}
	};
}
