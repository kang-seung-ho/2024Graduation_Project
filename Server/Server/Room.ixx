module;
#include <initializer_list>
#include <memory>
#include <vector>
#include <span>

export module Iconer.Application.Room;
import :RoomBase;
export import :RoomStates;
import Iconer.Utility.Constraints;
import Iconer.Utility.MovableAtomic;
export import Iconer.Application.NativeTimer;

export namespace iconer::app
{
	class [[nodiscard]] User;

	struct [[nodiscard]] RoomMember
	{
		iconer::util::MovableAtomic<User*> myHandle;
		iconer::util::MovableAtomic<bool> isReady;
	};

	class [[nodiscard]] Room : public detail::RoomBase
	{
	public:
		struct [[nodiscard]] MemberRemover
		{
			constexpr MemberRemover() noexcept = default;
			virtual constexpr ~MemberRemover() noexcept = default;

			virtual void operator()(volatile Room& room, const size_t& member_count) const = 0;
		};

		using Super = detail::RoomBase;
		using IdType = Super::IdType;
		//using MemberStorageType = iconer::collection::Array<RoomMember, maxUsersNumberInRoom>;
		//using iterator = MemberStorageType::iterator;
		//using const_iterator = MemberStorageType::const_iterator;
		using iterator = RoomMember*;
		using volatile_iterator = volatile RoomMember*;
		using const_iterator = const RoomMember*;
		using const_volatile_iterator = const volatile RoomMember*;

		static inline constexpr size_t maxUsersNumberInRoom = 4;
		static inline constexpr size_t minUsersNumberForGame = 2;

		template<typename ForwardedId>
		explicit constexpr Room(ForwardedId&& id)
			noexcept(nothrow_constructible<Super, const IdType&>)
			: Super(std::forward<ForwardedId>(id))
			, myMembers(), membersCount()
			, proceedMemberCount(), isGameReadyFailed()
			, myTimer()
			, preRespondMembersPacket()
		{
		}

		void Awake() noexcept;
		void Cleanup() noexcept;
		void Cleanup() volatile noexcept;

		void SetTimer(NativeTimer&& timer) noexcept
		{
			myTimer = std::exchange(timer, NativeTimer{});
		}

		bool UnsetTimer() noexcept
		{
			return std::exchange(myTimer, NativeTimer{}).Stop();
		}

		bool TryAddMember(iconer::app::User& user) volatile noexcept;
		bool RemoveMember(const IdType& id) volatile noexcept;
		bool RemoveMember(const IdType& id, function_t<void, volatile Room&, const size_t&> predicate) volatile;
		bool RemoveMember(const IdType& id, nothrow_function_t<void, volatile Room&, const size_t&> predicate) volatile noexcept;
		bool RemoveMember(const IdType& id, const MemberRemover& predicate) volatile;
		bool RemoveMember(const IdType& id, MemberRemover&& predicate) volatile;
		void ClearMembers() volatile noexcept;

		void Dirty(bool flag) volatile noexcept;
		[[nodiscard]] bool Dirty() const volatile noexcept;

		template<invocables<User&> Predicate>
		void ForEach(Predicate&& predicate) const
		{
			for (auto& [user, _] : myMembers)
			{
				if (nullptr != user.Load(std::memory_order_acquire))
				{
					std::invoke(std::forward<Predicate>(predicate), *user);
				}
			}
		}

		bool ReadyMember(iconer::app::User& user) volatile noexcept;
		bool UnreadyMember(iconer::app::User& user) volatile noexcept;

		bool StartGame();
		void CloseGame();

		[[nodiscard]] std::span<std::byte> SerializeMembers() volatile;

		[[nodiscard]] size_t GetMembersCount() const volatile noexcept;

		[[nodiscard]] bool IsEmpty() const volatile noexcept;
		[[nodiscard]] bool IsFull() const volatile noexcept;
		[[nodiscard]] bool HasMember(const IdType& id) const volatile noexcept;
		[[nodiscard]] bool IsEveryMemberIsLoaded() const volatile noexcept;
		[[nodiscard]] bool CanCreateGame() const volatile noexcept;
		[[nodiscard]] bool CanPrepareGame() const volatile noexcept;
		[[nodiscard]] bool CanStartGame() const volatile noexcept;

		[[nodiscard]]
		constexpr iterator begin() noexcept
		{
			return myMembers;
		}

		[[nodiscard]]
		constexpr iterator end() noexcept
		{
			return myMembers + maxUsersNumberInRoom;
		}

		[[nodiscard]]
		constexpr const_iterator begin() const noexcept
		{
			return myMembers;
		}

		[[nodiscard]]
		constexpr const_iterator end() const noexcept
		{
			return myMembers + maxUsersNumberInRoom;
		}

		[[nodiscard]]
		constexpr const_iterator cbegin() const noexcept
		{
			return myMembers;
		}

		[[nodiscard]]
		constexpr const_iterator cend() const noexcept
		{
			return myMembers + maxUsersNumberInRoom;
		}

		[[nodiscard]]
		constexpr volatile_iterator begin() volatile noexcept
		{
			return myMembers;
		}

		[[nodiscard]]
		constexpr volatile_iterator end() volatile noexcept
		{
			return myMembers + maxUsersNumberInRoom;
		}

		[[nodiscard]]
		constexpr const_volatile_iterator begin() const volatile noexcept
		{
			return myMembers;
		}

		[[nodiscard]]
		constexpr const_volatile_iterator end() const volatile noexcept
		{
			return myMembers + maxUsersNumberInRoom;
		}

		[[nodiscard]]
		constexpr const_volatile_iterator cbegin() const volatile noexcept
		{
			return myMembers;
		}

		[[nodiscard]]
		constexpr const_volatile_iterator cend() const volatile noexcept
		{
			return myMembers + maxUsersNumberInRoom;
		}

		RoomMember myMembers[maxUsersNumberInRoom];
		iconer::util::MovableAtomic<size_t> membersCount;
		iconer::util::MovableAtomic<bool> isMemberUpdated;
		iconer::util::MovableAtomic<size_t> proceedMemberCount;
		iconer::util::MovableAtomic<bool> isGameReadyFailed;
		NativeTimer myTimer;

	protected:
		std::unique_ptr<std::byte[]> preRespondMembersPacket;
	};
}
