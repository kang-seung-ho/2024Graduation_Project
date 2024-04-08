export module Iconer.Application.Room:RoomBase;
export import :RoomStates;
import Iconer.Utility.Constraints;
import Iconer.Application.ISession;

export namespace iconer::app::detail
{
	class [[nodiscard]] RoomBase : public ISession<RoomStates>
	{
	public:
		using Super = iconer::app::ISession<RoomStates>;
		using Super::IdType;

		template<typename ForwardedId>
		constexpr RoomBase(ForwardedId&& id)
			noexcept(nothrow_constructible<Super, ForwardedId>)
			: Super(std::forward<ForwardedId>(id))
		{}

		bool TryReserveContract() volatile noexcept
		{
			return TryChangeState(RoomStates::None, RoomStates::Reserved);
		}

		bool TryCancelContract() volatile noexcept
		{
			return TryChangeState(RoomStates::Reserved, RoomStates::None);
		}

		bool TryBeginCreate() volatile noexcept
		{
			return TryChangeState(RoomStates::Reserved, RoomStates::Creating);
		}

		bool TryCancelCreating() volatile noexcept
		{
			return TryChangeState(RoomStates::Creating, RoomStates::None);
		}

		bool TryEstablish() volatile noexcept
		{
			return TryChangeState(RoomStates::Creating, RoomStates::Idle);
		}

		bool TryCancelEstablishing() volatile noexcept
		{
			return TryChangeState(RoomStates::Idle, RoomStates::None);
		}

		bool TryGettingReady() volatile noexcept
		{
			return TryChangeState(RoomStates::Idle, RoomStates::Ready);
		}

		bool TryCancelReady(RoomStates next_state = RoomStates::Idle) volatile noexcept
		{
			return TryChangeState(RoomStates::Ready, next_state);
		}

		bool TryMarkPreparedForGame() volatile noexcept
		{
			return TryChangeState(RoomStates::Ready, RoomStates::Prepared);
		}

		bool TryUnmarkPreparedForGame(RoomStates next_state) volatile noexcept
		{
			return TryChangeState(RoomStates::InGame, next_state);
		}

		bool TryBeginGame() volatile noexcept
		{
			return TryChangeState(RoomStates::Prepared, RoomStates::InGame);
		}

		bool TryCancelBeginGame(RoomStates next_state = RoomStates::Idle) volatile noexcept
		{
			return TryChangeState(RoomStates::Prepared, next_state);
		}

		bool TryCancelBeginGameInGame(RoomStates next_state = RoomStates::Idle) volatile noexcept
		{
			return TryChangeState(RoomStates::InGame, next_state);
		}

		void BeginClose() volatile noexcept
		{
			SetState(RoomStates::Closing);
		}

		bool TryBeginClose(RoomStates prev_state) volatile noexcept
		{
			return TryChangeState(prev_state, RoomStates::Closing);
		}

		bool TryEndClose(RoomStates next_state = RoomStates::None) volatile noexcept
		{
			return TryChangeState(RoomStates::Closing, next_state);
		}
	};
}
