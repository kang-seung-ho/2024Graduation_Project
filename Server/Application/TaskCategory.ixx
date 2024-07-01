export module Iconer.App.TaskCategory;
import <cstdint>;

export namespace iconer::app
{
	enum class [[nodiscard]] TaskCategory : std::uint8_t
	{
		None = 0,

		/// <summary>Phase 0</summary>
		OpReserve,

		/// <summary>Phase 1</summary>
		OpAccept, OpValidation,

		/// <summary>Phase 2 ~ </summary>
		OpOptainRecvMemory, OpRecv, OpSend, OpSendBorrowed,

		/// <summary>Phase 3 </summary>
		OpPacketProcess,

		/// <summary>Phase 4 - Sign in</summary>
		OpSignIn, OpSignInFailed, OpSignUp, OpSignUpFailed,

		/// <summary>Phase 5</summary>
		OpAssignID,

		/// <summary>Phase 6 </summary>
		OpReserveRoom, OpCreateRoom, OpEnterRoom, OpLeaveRoom, OpCloseRoom,

		/// <summary>Phase 7 </summary>
		OpNotifyMember, OpNotifyRoom, OpNotifyTeam,

		/// <summary>Phase 8 </summary>
		OpCreateGame, OpSpreadGameTicket, OpReadyGame, OpGameTicketing, OpStartGame, OpLeaveGame, OpCloseGame,

		/// <summary>Phase 9 ~ </summary>
		OpCreateCharacters, OpUpdateRoom, OpCheckGuardian, OpCheckDead,

		/// <summary>Phase 10 ~ </summary>
		OpRpc, OpSendRpc, OpCleanRpc,

		/// <summary>Phase 11 - Sign out (Quit)</summary>
		OpClose,

		/// <summary>Extras</summary>
		OpEndWorkers
	};
}
