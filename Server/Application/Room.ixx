export module Iconer.App.Room;
import Iconer.Utility.ReadOnly;
import Iconer.Utility.Delegate;
import Iconer.Utility.Container.AtomicQueue;
import Iconer.Net.ErrorCode;
import Iconer.App.ISession;
import Iconer.App.TaskContext;
import Iconer.App.Settings;
import <string_view>;
import <string>;
import <array>;
import <atomic>;

export namespace iconer::app
{
	class [[nodiscard]] User;

	class [[nodiscard]] Member
	{
	private:
		using pointer = iconer::app::User*;

		std::atomic<pointer> userHandle{};

	public:
		Member() noexcept = default;

		[[nodiscard]]
		bool TryJoin(pointer user) noexcept
		{
			pointer null = nullptr;
			return userHandle.compare_exchange_strong(null, user);
		}

		[[nodiscard]]
		bool ChangedToEmpty(pointer before_user, std::memory_order order = std::memory_order_release) noexcept
		{
			return userHandle.compare_exchange_strong(before_user, nullptr, order);
		}

		[[nodiscard]]
		pointer GetStoredUser(std::memory_order order = std::memory_order_acquire) const volatile noexcept
		{
			return userHandle.load(order);
		}

		[[nodiscard]]
		bool IsAvailable() const noexcept
		{
			return nullptr != userHandle;
		}
	};

	class [[nodiscard]] Room : public ISession
	{
	public:
		using super = ISession;
		using this_class = Room;
		using id_type = super::id_type;
		using session_type = Member;
		using pointer_type = iconer::app::User*;
		using reference = iconer::app::User&;
		using const_reference = const iconer::app::User&;
		using size_type = std::int32_t;

		static inline constexpr size_t userLimits = Settings::roomMembersLimit;
		static inline constexpr size_t titleLength = Settings::roomTitleLength;

		iconer::util::Delegate<void, this_class*, pointer_type> onOccupied{};
		iconer::util::Delegate<void, this_class*> onDestroyed{};
		iconer::util::Delegate<void, this_class*, pointer_type, size_type> onUserJoined{};
		iconer::util::Delegate<void, this_class*, pointer_type, size_type> onUserLeft{};
		iconer::util::Delegate<void, this_class*, pointer_type> onFailedToJoinUser{};

		explicit constexpr Room(id_type id) noexcept
			: super(id)
		{
			myTitle.resize(titleLength * 2);
		}

		~Room() = default;

		[[nodiscard]] bool TryOccupy(reference user);
		[[nodiscard]] bool TryJoin(reference user);
		bool Leave(reference user, bool notify = true) noexcept;

		template<invocable<reference> Callable>
		constexpr void Foreach(Callable&& fun) const noexcept(nothrow_invocable<Callable, reference>)
		{
			for (const session_type& member : myMembers)
			{
				const pointer_type ptr = member.GetStoredUser();

				std::forward<Callable>(fun)(*ptr);
			}
		}

		template<invocable<const_reference> Callable>
		constexpr void Foreach(Callable&& fun) const noexcept(nothrow_invocable<Callable, const_reference>)
		{
			for (const session_type& member : myMembers)
			{
				const pointer_type ptr = member.GetStoredUser();

				std::forward<Callable>(fun)(*ptr);
			}
		}

		[[nodiscard]]
		constexpr void SetTitle(std::wstring_view title) noexcept
		{
			if (0 < title.length())
			{
				myTitle = title;
			}
		}

		[[nodiscard]]
		constexpr std::wstring_view GetTitle() const noexcept
		{
			return myTitle.data();
		}

		[[nodiscard]]
		std::int32_t GetMemberCount() const noexcept
		{
			return memberCount.load(std::memory_order_acquire);
		}

		[[nodiscard]]
		bool IsTaken() const noexcept
		{
			return isTaken.load(std::memory_order_acquire);
		}

	private:
		std::atomic_bool isTaken{};
		std::wstring myTitle{};
		std::array<session_type, userLimits> myMembers{};
		std::atomic<size_type> memberCount{};

		[[nodiscard]]
		bool ChangedIsTaken(bool before, bool flag, std::memory_order order = std::memory_order_release) noexcept
		{
			return isTaken.compare_exchange_strong(before, flag, order);
		}
	};
}
