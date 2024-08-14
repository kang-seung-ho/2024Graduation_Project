export module Iconer.App.Room;
import Iconer.Utility.TypeTraits;
import Iconer.Utility.ReadOnly;
import Iconer.Utility.Delegate;
import Iconer.Utility.Container.AtomicQueue;
import Iconer.Net.ErrorCode;
import Iconer.App.ISession;
import Iconer.App.TaskContext;
import Iconer.App.GameSession;
import Iconer.App.PacketSerializer;
import Iconer.App.RpcProtocols;
import Iconer.App.Settings;
import <chrono>;
import <span>;
import <string_view>;
import <string>;
import <array>;
import <atomic>;

export namespace iconer::app
{
	class [[nodiscard]] SagaPlayer
	{
	public:
		using pointer_type = class User*;
		using reference = class User&;
		using const_reference = class User const&;

		static inline constexpr float maxHp = 100;

		std::atomic<pointer_type> userHandle{};
		std::atomic<ESagaPlayerTeam> myTeamId{};
		std::atomic_uint8_t myWeapon{};
		std::atomic_bool isReady{};
		std::atomic<float> myHp{ maxHp };
		std::atomic_int32_t ridingGuardianId{ -1 };
		std::chrono::system_clock::time_point respawnTime{};

		SagaPlayer() noexcept = default;

		[[nodiscard]]
		bool TryJoin(pointer_type user) noexcept
		{
			pointer_type null = nullptr;

			return userHandle.compare_exchange_strong(null, user);
		}

		[[nodiscard]]
		bool ChangedToEmpty(pointer_type before_user, std::memory_order order = std::memory_order_release) noexcept
		{
			return userHandle.compare_exchange_strong(before_user, nullptr, order);
		}

		[[nodiscard]]
		bool TryReady() noexcept
		{
			bool before = false;
			return isReady.compare_exchange_strong(before, true);
		}

		bool UnReady() noexcept
		{
			bool before = true;
			return isReady.compare_exchange_strong(before, false);
		}

		template<invocable<reference> Callable>
		bool ProcessMember(pointer_type user, Callable&& fun) const noexcept(nothrow_invocable<Callable, reference>)
		{
			const auto mem_ptr = GetStoredUser();

			if (nullptr != mem_ptr and user == mem_ptr)
			{
				std::forward<Callable>(fun)(*user);

				return true;
			}
			else
			{
				return false;
			}
		}

		template<invocable<const_reference> Callable>
		bool ProcessMember(pointer_type user, Callable&& fun) const noexcept(nothrow_invocable<Callable, const_reference>)
		{
			const auto mem_ptr = GetStoredUser();

			if (nullptr != mem_ptr and user == mem_ptr)
			{
				std::forward<Callable>(fun)(*user);

				return true;
			}
			else
			{
				return false;
			}
		}

		[[nodiscard]]
		pointer_type GetStoredUser(std::memory_order order = std::memory_order_acquire) const volatile noexcept
		{
			return userHandle.load(order);
		}

		[[nodiscard]]
		bool IsAvailable() const noexcept
		{
			return nullptr != userHandle;
		}
	};

	struct [[nodiscard]] SerializedMember
	{
		static inline constexpr size_t nameLength = 16;

		std::int32_t id{ -1 };
		char myTeamId{ 0 }; // 1: red, 2: blue
		std::array<wchar_t, nameLength + 1> nickname{};

		[[nodiscard]]
		static consteval size_t GetSerializedSize() noexcept
		{
			return sizeof(std::int32_t) + sizeof(wchar_t) * nameLength + sizeof(char);
		}
	};

	enum class [[nodiscard]] RoomState
	{
		Idle, PrepareGame, Gaming
	};

	class [[nodiscard]] Room : public ISession
	{
	public:
		using super = ISession;
		using this_class = Room;
		using id_type = super::id_type;
		using session_type = SagaPlayer;
		using pointer_type = session_type::pointer_type;
		using reference = session_type::reference;
		using const_reference = session_type::const_reference;
		using size_type = std::int32_t;

		static inline constexpr size_t userLimits = Settings::roomMembersLimit;
		static inline constexpr size_t titleLength = Settings::roomTitleLength;
		static inline constexpr std::int16_t maxSerializeMemberDataSize = 3 + SerializedMember::GetSerializedSize() * userLimits + sizeof(size_t);

		std::atomic<RoomState> myState;
		std::atomic_int32_t startingMemberProceedCount;
		std::atomic_int32_t readyMemberProceedCount;
		std::atomic_int32_t gameLoadedMemberProceedCount;
		std::chrono::system_clock::time_point selectionPhaseTime;
		std::chrono::system_clock::time_point gamePhaseTime;

		// equals to team id
		std::atomic_int8_t sagaWinner{};
		std::atomic_int32_t sagaTeamScores[2]{};
		SagaGuardian sagaGuardians[3]{};
		std::atomic_bool sagaItemListLock{};
		std::array<SagaItem, 200> sagaItemList{};
		std::atomic_size_t sagaItemListSize{};

		iconer::util::Delegate<void, this_class*, pointer_type> onOccupied{};
		iconer::util::Delegate<void, this_class*> onDestroyed{};
		iconer::util::Delegate<void, this_class*, pointer_type, size_type> onUserJoined{};
		iconer::util::Delegate<void, this_class*, pointer_type, size_type> onUserLeft{};
		iconer::util::Delegate<void, this_class*, pointer_type> onFailedToJoinUser{};

		explicit constexpr Room(id_type id) noexcept
			: super(id)
		{
			myTitle.resize(titleLength * 2);

			iconer::app::SerializeAt(weaponChoiceTimerPacketData, PacketProtocol::SC_RPC, 0, RpcProtocol::RPC_WEAPON_TIMER, 0LL, 0);
			iconer::app::SerializeAt(gameTimerPacketData, PacketProtocol::SC_RPC, 0, RpcProtocol::RPC_GAME_TIMER, 0LL, 0);
			iconer::app::SerializeAt(gameScorePacketData, PacketProtocol::SC_RPC, 0, RpcProtocol::RPC_GET_SCORE, 0LL, 0);
		}

		~Room() = default;

		void Initialize();
		void Cleanup();

		[[nodiscard]] bool TryOccupy(reference user);
		[[nodiscard]] bool TryJoin(reference user);
		bool Leave(reference user, bool notify = true) noexcept;

		[[nodiscard]]
		bool TryStartGame() noexcept
		{
			RoomState state{ RoomState::Idle };
			return myState.compare_exchange_strong(state, RoomState::PrepareGame, std::memory_order_acq_rel);
		}

		bool CancelStartGame() noexcept
		{
			RoomState state{ RoomState::PrepareGame };
			return myState.compare_exchange_strong(state, RoomState::Idle, std::memory_order_acq_rel);
		}

		template<invocable<session_type&> Callable>
		bool ProcessMember(pointer_type user, Callable&& fun) noexcept(nothrow_invocable<Callable, session_type&>)
		{
			for (session_type& member : myMembers)
			{
				const pointer_type ptr = member.GetStoredUser();

				if (nullptr != ptr and user == ptr)
				{
					std::forward<Callable>(fun)(member);

					return true;
				}
			}

			return false;
		}

		template<invocable<const session_type&> Callable>
		bool ProcessMember(pointer_type user, Callable&& fun) const noexcept(nothrow_invocable<Callable, const session_type&>)
		{
			for (const session_type& member : myMembers)
			{
				const pointer_type ptr = member.GetStoredUser();

				if (nullptr != ptr and user == ptr)
				{
					std::forward<Callable>(fun)(member);

					return true;
				}
			}

			return false;
		}

		template<invocable<session_type&> Callable>
		constexpr void Foreach(Callable&& fun) noexcept(nothrow_invocable<Callable, session_type&>)
		{
			for (session_type& member : myMembers)
			{
				std::forward<Callable>(fun)(member);
			}
		}

		template<invocable<const session_type&> Callable>
		constexpr void Foreach(Callable&& fun) const noexcept(nothrow_invocable<Callable, const session_type&>)
		{
			for (const session_type& member : myMembers)
			{
				std::forward<Callable>(fun)(member);
			}
		}

		template<invocable<reference> Callable>
		constexpr void Foreach(Callable&& fun) const noexcept(nothrow_invocable<Callable, reference>)
		{
			for (const session_type& member : myMembers)
			{
				const pointer_type ptr = member.GetStoredUser();

				if (nullptr == ptr) continue;

				std::forward<Callable>(fun)(*ptr);
			}
		}

		template<invocable<const_reference> Callable>
		constexpr void Foreach(Callable&& fun) const noexcept(nothrow_invocable<Callable, const_reference>)
		{
			for (const session_type& member : myMembers)
			{
				const pointer_type ptr = member.GetStoredUser();

				if (nullptr == ptr) continue;

				std::forward<Callable>(fun)(*ptr);
			}
		}

		void SetMemberTeam(const_reference user, bool is_red_team);

		constexpr void SetTitle(std::wstring_view title)
		{
			myTitle = title;
		}

		[[nodiscard]] std::span<const std::byte> MakeMemberListPacket();
		[[nodiscard]] std::pair<std::unique_ptr<std::byte[]>, std::int16_t> MakeMemberJoinedPacket(const_reference user) const;

		[[nodiscard]]
		auto& MakeWeaponChoiceTimerPacket(const std::int64_t& time) noexcept
		{
			while (true)
			{
				bool flag = false;

				if (weaponChoiceTimerPacketAcquired.compare_exchange_strong(flag, true, std::memory_order_acquire))
				{
					break;
				}
			}

			std::memcpy(weaponChoiceTimerPacketData.data() + 8, &time, sizeof(std::int64_t));

			bool rollback = true;
			weaponChoiceTimerPacketAcquired.compare_exchange_strong(rollback, false, std::memory_order_release);

			return weaponChoiceTimerPacketData;
		}

		[[nodiscard]]
		auto& MakeGameTimerPacket(const std::int64_t& time) noexcept
		{
			std::memcpy(gameTimerPacketData.data() + 8, &time, sizeof(std::int64_t));

			return gameTimerPacketData;
		}

		[[nodiscard]]
		auto& MakeGameScorePacket(const std::int64_t& red_score, const std::int32_t& blu_score) noexcept
		{
			std::memcpy(gameScorePacketData.data() + 8, &red_score, sizeof(std::int64_t));
			std::memcpy(gameScorePacketData.data() + 16, &blu_score, sizeof(std::int32_t));

			return gameScorePacketData;
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

		[[nodiscard]]
		bool IsIdle() const noexcept
		{
			return RoomState::Idle == myState.load(std::memory_order_acquire);
		}

		[[nodiscard]]
		bool IsStartingGame() const noexcept
		{
			return RoomState::PrepareGame == myState.load(std::memory_order_acquire);
		}
		
		[[nodiscard]]
		bool IsGameEnded() const noexcept
		{
			return 0 != sagaWinner.load(std::memory_order_acquire);
		}

	private:
		std::atomic_bool isTaken{};
		std::wstring myTitle{};
		alignas(std::hardware_constructive_interference_size) std::array<session_type, userLimits> myMembers{};
		std::atomic<size_type> memberCount{};
		std::array<std::byte, maxSerializeMemberDataSize> precachedMemberListData{};
		size_t precachedMemberListDataSize{};
		std::atomic_bool isDirty{};

		// 137[1] => SC_RPC
		// 20|0[2] => size
		// 0[4] => id (nothing)
		// 225[1] => RPC_WEAPON_TIMER
		// [8] => seconds count
		std::array<std::byte, 20> weaponChoiceTimerPacketData{};
		// 137[1] => SC_RPC
		// 20|0[2] => size
		// 0[4] => id (nothing)
		// 225[1] => RPC_GAME_TIMER
		// [8] => seconds count
		std::array<std::byte, 20> gameTimerPacketData{};
		// 137[1] => SC_RPC
		// 20|0[2] => size
		// 0[4] => id (nothing)
		// 222[1] => RPC_GET_SCORE
		std::array<std::byte, 20> gameScorePacketData{};
		std::atomic_bool weaponChoiceTimerPacketAcquired{};

		[[nodiscard]]
		bool ChangedIsTaken(bool before, bool flag, std::memory_order order = std::memory_order_release) noexcept
		{
			return isTaken.compare_exchange_strong(before, flag, order);
		}

		Room(const Room&) = delete;
		Room& operator=(const Room&) = delete;
		Room(Room&&) = delete;
		Room& operator=(Room&&) = delete;
	};
}
