export module Iconer.App.TaskContext;
import Iconer.Utility.TypeTraits;
import Iconer.Net.IoContext;
import Iconer.Net.Socket;
import <atomic>;

export namespace iconer::app
{
	enum class [[nodiscard]] TaskCategory
	{
		None = 0,

		/// <summary>Phase 0</summary>
		OpReserve,

		/// <summary>Phase 1</summary>
		OpAccept, OpValidation,

		/// <summary>Phase 2 - Sign in</summary>
		OpSignIn, OpSignInFailed, OpSignUp, OpSignUpFailed,

		/// <summary>Phase 3</summary>
		OpAssignID,

		/// <summary>Phase 4 ~ </summary>
		OpRecv, OpSend, OpSendBorrowed,

		/// <summary>Phase 5 </summary>
		OpReserveRoom, OpCreateRoom, OpEnterRoom, OpLeaveRoom, OpCloseRoom,

		/// <summary>Phase 6 </summary>
		OpNotifyMember, OpNotifyRoom, OpNotifyTeam,

		/// <summary>Phase 7 </summary>
		OpCreateGame, OpSpreadGameTicket, OpReadyGame, OpGameTicketing, OpStartGame, OpLeaveGame, OpCloseGame,

		/// <summary>Phase 8 ~ </summary>
		OpCreateCharacters, OpUpdateRoom, OpCheckGuardian, OpCheckDead,
		/// <summary>Phase 8 ~ </summary>
		OpRpc, OpSendRpc, OpCleanRpc,

		/// <summary>Phase 10 - Sign out (Quit)</summary>
		OpDisconnect,

		/// <summary>Extras</summary>
		OpEndWorkers
	};

	class [[nodiscard]] TaskContext : public iconer::net::IoContext
	{
	public:
		std::atomic<TaskCategory> myCategory;
		std::atomic_bool isOccupied;

		constexpr TaskContext() noexcept = default;
		constexpr ~TaskContext() noexcept = default;

		constexpr TaskContext(TaskCategory category) noexcept
			: myCategory(category)
			, isOccupied()
		{}

		void SetOperation(TaskCategory op, std::memory_order order = std::memory_order_seq_cst) noexcept
		{
			myCategory.store(op, order);
		}

		bool TryChangeOperation(TaskCategory from, TaskCategory to, std::memory_order order = std::memory_order_seq_cst) noexcept
		{
			return myCategory.compare_exchange_strong(from, to, order);
		}

		template<invocable Fn>
		TaskContext& and_then(TaskCategory op, Fn&& fun, std::memory_order order = std::memory_order_acquire) noexcept(nothrow_invocable<Fn>)
		{
			auto prior = myCategory.load(order);

			while (true)
			{
				bool expected = false;

				if (isOccupied.compare_exchange_strong(expected, true, std::memory_order_acquire))
				{
					if (op == prior)
					{
						(std::forward<Fn>(fun))();
					}

					bool rollback = true;
					isOccupied.compare_exchange_strong(rollback, false, std::memory_order_release);

					break;
				}
			}
		}

		template<invocable Fn>
		TaskContext& or_else(TaskCategory op, Fn&& fun, std::memory_order order = std::memory_order_acquire) noexcept(nothrow_invocable<Fn>)
		{
			auto prior = myCategory.load(order);

			while (true)
			{
				bool expected = false;

				if (isOccupied.compare_exchange_strong(expected, true, std::memory_order_acquire))
				{
					if (op != prior)
					{
						(std::forward<Fn>(fun))();
					}

					bool rollback = true;
					isOccupied.compare_exchange_strong(rollback, false, std::memory_order_release);

					break;
				}
			}
		}
	};
}
