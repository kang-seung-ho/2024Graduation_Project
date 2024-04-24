module;
#include <utility>
#include <vector>
#include <span>
#include <ranges>

module Iconer.Application.Room;
import Iconer.Utility.Constraints;
import Iconer.Application.User;
import Iconer.Application.Packet;

namespace iconer::app
{
	template<typename Predicate>
	bool Remover(volatile Room& room, const iconer::app::User::IdType& user_id, Predicate&& predicate)
		noexcept(nothrow_invocables<Predicate, volatile Room&, size_t>)
	{
		bool result = false;
		auto capture = room.membersCount.Load(std::memory_order_acquire);

		auto it = room.begin();
		auto end = room.end();

		while (it != end)
		{
			auto& [member, is_ready] = *it;
			if (nullptr != member and user_id == member->GetID())
			{
				is_ready.CompareAndSet(true, false);
				room.Dirty(true);

				if (0 < capture)
				{
					--capture;
				}

				std::invoke(std::forward<Predicate>(predicate), room, capture);

				member = nullptr;
				room.membersCount.Store(capture, std::memory_order_release);

				return true;
			}

			(void)++it;
		}

		room.membersCount.Store(capture, std::memory_order_release);
		return false;
	}
}

void
iconer::app::Room::Awake()
noexcept
{
	preRespondMembersPacket = std::make_unique_for_overwrite<std::byte[]>(packets::SC_RespondMembersPacket::MaxSize());
}

void
iconer::app::Room::Cleanup()
noexcept
{
	ClearMembers();
	Clear();
	SetState(RoomStates::None);
	SetOperation(AsyncOperations::None);
	membersCount = 0;
	isMemberUpdated = true;
	proceedMemberCount = 0;
	isGameReadyFailed = false;
}

void
iconer::app::Room::Cleanup()
volatile noexcept
{
	ClearMembers();
	Clear();
	SetState(RoomStates::None);
	SetOperation(AsyncOperations::None);
	membersCount = 0;
	isMemberUpdated = true;
	proceedMemberCount = 0;
	isGameReadyFailed = false;
}


bool
iconer::app::Room::TryAddMember(iconer::app::User& user)
volatile noexcept
{
	bool result = false;
	auto capture = membersCount.Load(std::memory_order_acquire);

	if (capture < maxUsersNumberInRoom)
	{
		for (auto& [member, _] : myMembers)
		{
			if (member.CompareAndSet(nullptr, std::addressof(user), std::memory_order_acq_rel))
			{
				++capture;

				Dirty(true);

				result = true;
				break;
			}
		}
	}

	membersCount.Store(capture, std::memory_order_release);
	return result;
}

bool
iconer::app::Room::RemoveMember(const iconer::app::Room::IdType& id)
volatile noexcept
{
	static constexpr auto temp_pred = [](volatile Room&, size_t) noexcept {};

	return Remover(*this, id, temp_pred);
}

bool
iconer::app::Room::RemoveMember(const IdType& id, function_t<void, volatile Room&, const size_t&> predicate)
volatile
{
	return Remover(*this, id, predicate);
}

bool
iconer::app::Room::RemoveMember(const IdType& id, nothrow_function_t<void, volatile Room&, const size_t&> predicate)
volatile noexcept
{
	return Remover(*this, id, predicate);
}

bool
iconer::app::Room::RemoveMember(const iconer::app::Room::IdType& id, const iconer::app::Room::MemberRemover& predicate)
volatile
{
	return Remover(*this, id, predicate);
}

bool
iconer::app::Room::RemoveMember(const iconer::app::Room::IdType& id, iconer::app::Room::MemberRemover&& predicate)
volatile
{
	return Remover(*this, id, std::move(predicate));
}

bool
iconer::app::Room::ReadyMember(iconer::app::User& user)
volatile noexcept
{
	for (auto& [member, is_ready] : myMembers)
	{
		if (nullptr != member)
		{
			if (user.GetID() == member->GetID())
			{
				if (is_ready.CompareAndSet(false, true))
				{
					return true;
				}
				else if (is_ready)
				{
					return true;
				}
			}
		}
		else
		{
			is_ready.CompareAndSet(true, false);
		}
	}

	return false;
}

bool
iconer::app::Room::UnreadyMember(iconer::app::User& user)
volatile noexcept
{
	for (auto& [member, is_ready] : myMembers)
	{
		if (nullptr != member)
		{
			if (user.GetID() == member->GetID())
			{
				if (is_ready.CompareAndSet(true, false))
				{
					return true;
				}
				else if (is_ready)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool
iconer::app::Room::StartGame()
{
	if (CanStartGame() and TryBeginGame())
	{
		const auto max_cnt = proceedMemberCount.Load(std::memory_order_acquire);
		size_t cnt = 0;

		for (auto& [user, _] : myMembers)
		{
			if (auto ptr = user.Load(std::memory_order_acquire); nullptr != ptr)
			{
				user->SetState(UserStates::InGame);
				auto [io, ctx] = user->SendGameJustStartedPacket();
				if (not io)
				{
					ctx.Complete();
				}

				cnt = proceedMemberCount.FetchAdd(1, std::memory_order_release);
			}
		}

		SetOperation(AsyncOperations::OpCreateCharacters);

		//return proceedMemberCount.CompareAndSet(cnt - 1, 0, std::memory_order_release);
		return true;
	}
	else
	{
		TryCancelBeginGame();
		return false;
	}
}

void
iconer::app::Room::CloseGame()
{
	SetOperation(AsyncOperations::None);
	proceedMemberCount = 0;
	isGameReadyFailed = false;

	if (0 < GetMembersCount())
	{
		if (TryEndClose(RoomStates::Idle))
		{
			ForEach
			(
				[](User& member)
				{
					member.TryChangeState(UserStates::MakingGame, UserStates::InRoom);
					member.TryChangeState(UserStates::ReadyForGame, UserStates::InRoom);
				}
			);
		}
	}
	else
	{
		if (TryEndClose(RoomStates::None))
		{
			membersCount = 0;
			isMemberUpdated = true;
		}
	}
}

void
iconer::app::Room::ClearMembers()
volatile noexcept
{
	for (auto& [member, is_ready] : myMembers)
	{
		member = nullptr;
		is_ready = false;
	}

	membersCount = 0;
	Dirty(true);
}

void
iconer::app::Room::Dirty(bool flag)
volatile noexcept
{
	isMemberUpdated = flag;
}

bool
iconer::app::Room::Dirty()
const volatile noexcept
{
	return isMemberUpdated;
}

std::span<std::byte>
iconer::app::Room::SerializeMembers()
volatile
{
	packets::SC_RespondMembersPacket pk{};
	pk.serializedMembers.reserve(maxUsersNumberInRoom);

	auto ptr = const_cast<Room*>(this)->preRespondMembersPacket.get();

	if (Dirty())
	{
		pk.serializedMembers.clear();
		for (auto& [member, _] : myMembers)
		{
			if (nullptr != member)
			{
				pk.AddMember(member->GetID(), member->GetName(), member->myTeamId == Team::Red);
			}
		}

		pk.Write(ptr);
		Dirty(false);

		return std::span<std::byte>{ ptr, pk.WannabeSize() };
	}
	else
	{
		return std::span<std::byte>{ ptr, packets::SC_RespondMembersPacket::MinSize() + sizeof(iconer::app::packets::datagrams::SerializedMember) * membersCount + sizeof(size_t) };
	}
}

size_t
iconer::app::Room::GetMembersCount()
const volatile noexcept
{
	return membersCount.Load(std::memory_order_relaxed);
}

bool
iconer::app::Room::IsFull()
const volatile noexcept
{
	return maxUsersNumberInRoom <= membersCount.Load(std::memory_order_relaxed);
}

bool
iconer::app::Room::IsEmpty()
const volatile noexcept
{
	return 0 == membersCount.Load(std::memory_order_relaxed);
}

bool
iconer::app::Room::IsEveryMemberIsLoaded()
const volatile noexcept
{
	if (membersCount == 0)
	{
		return false;
	}

	if (membersCount < minUsersNumberForGame)
	{
		return false;
	}

	size_t readies = 0;
	for (auto& [member, is_ready] : myMembers)
	{
		if (is_ready)
		{
			readies++;
		}
	}

	return membersCount <= readies;
}

bool
iconer::app::Room::HasMember(const iconer::app::Room::IdType& id)
const volatile noexcept
{
	for (auto& [member, _] : myMembers)
	{
		if (nullptr != member and id == member->GetID())
		{
			return true;
		}
	}

	return false;
}

#define MUST_START true

bool
iconer::app::Room::CanCreateGame()
const volatile noexcept
{
	return GetState() == iconer::app::RoomStates::Idle and 0 < membersCount;
}

bool
iconer::app::Room::CanPrepareGame()
const volatile noexcept
{
	return GetState() == iconer::app::RoomStates::Ready and 0 < membersCount;
}

bool
iconer::app::Room::CanStartGame()
const volatile noexcept
{
#if MUST_START
	return GetState() == iconer::app::RoomStates::Prepared and 0 < membersCount;
#else
	return GetState() == iconer::app::RoomStates::Ready and minUsersNumberForGame <= membersCount and IsEveryMemberIsLoaded();
#endif
}

struct RoomProcedure : public iconer::app::TimerProcedure
{
	using Super = iconer::app::TimerProcedure;

	RoomProcedure(iconer::app::Room* room, std::uint32_t milliseconds)
		: Super(room->myTimer, milliseconds, true)
		, myRoom(room)
	{}

	virtual bool operator()() override
	{
		//std::println("Update room {}...", myRoom->GetID());

		return true;
	}

	iconer::app::Room* myRoom;
};
