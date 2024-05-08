export module Iconer.Application.AsyncOperation;
import <cstdint>;
import <string>;

export namespace iconer::app
{
	enum class [[nodiscard]] AsyncOperations : std::uint8_t
	{
		None,

		/// <summary>Phase 0</summary>
		OpReserveSession = 10,

		/// <summary>Phase 1</summary>
		OpAccept,

		/// <summary>Phase 2 - Sign in</summary>
		OpSignIn, OpSignInFailed,

		/// <summary>Phase 3</summary>
		OpAssignID,

		/// <summary>Phase 4 ~ </summary>
		OpRecv, OpSend, OpSendBorrowed, OpSendUniqueContext,

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
}

export namespace std
{
	[[nodiscard]]
	constexpr string to_string(const iconer::app::AsyncOperations& op)
	{
		using enum iconer::app::AsyncOperations;
		switch (op)
		{
			case None: return "None";
			case OpReserveSession: return "ReserveSession";
			case OpAccept: return "Accept";
			case OpSignIn: return "SignIn";
			case OpSignInFailed: return "SignInFailed";
			case OpAssignID: return "AssignID";
			case OpRecv: return "Recv";
			case OpSend: return "Send";
			case OpSendBorrowed: return "SendBorrowed";
			case OpSendUniqueContext: return "SendUniqueContext";
			case OpReserveRoom: return "ReserveRoom";
			case OpCreateRoom: return "CreateRoom";
			case OpEnterRoom: return "EnterRoom";
			case OpLeaveRoom: return "LeaveRoom";
			case OpCloseRoom: return "CloseRoom";
			case OpNotifyMember: return "NotifyMember";
			case OpNotifyRoom: return "Notifyroom";
			case OpNotifyTeam: return "NotifyTeam";
			case OpCreateGame: return "EnterGame";
			case OpSpreadGameTicket: return "SpreadGameTicket";
			case OpReadyGame: return "ReadyGame";
			case OpGameTicketing: return "GameTicketing";
			case OpStartGame: return "StartGame";
			case OpLeaveGame: return "LeaveGame";
			case OpCloseGame: return "CloseGame";
			case OpCreateCharacters: return "CreateCharacters";
			case OpUpdateRoom: return "UpdateRoom";
			case OpCheckGuardian: return "CheckGuardian";
			case OpCheckDead: return "CheckDead";
			case OpRpc: return "Rpc";
			case OpSendRpc: return "SendRpc";
			case OpDisconnect: return "Disconnect";
			case OpEndWorkers: return "EndWorkers";
			default: return "Unknown";
		}
	}

	[[nodiscard]]
	constexpr wstring to_wstring(const iconer::app::AsyncOperations& op)
	{
		using enum iconer::app::AsyncOperations;
		switch (op)
		{
			case None: return L"None";
			case OpReserveSession: return L"ReserveSession";
			case OpAccept: return L"Accept";
			case OpSignIn: return L"SignIn";
			case OpSignInFailed: return L"SignInFailed";
			case OpAssignID: return L"AssignID";
			case OpRecv: return L"Recv";
			case OpSend: return L"Send";
			case OpSendBorrowed: return L"SendBorrowed";
			case OpSendUniqueContext: return L"SendUniqueContext";
			case OpReserveRoom: return L"ReserveRoom";
			case OpCreateRoom: return L"CreateRoom";
			case OpEnterRoom: return L"EnterRoom";
			case OpLeaveRoom: return L"LeaveRoom";
			case OpCloseRoom: return L"CloseRoom";
			case OpNotifyMember: return L"NotifyMember";
			case OpNotifyRoom: return L"Notifyroom";
			case OpNotifyTeam: return L"NotifyTeam";
			case OpCreateGame: return L"EnterGame";
			case OpSpreadGameTicket: return L"SpreadGameTicket";
			case OpReadyGame: return L"ReadyGame";
			case OpGameTicketing: return L"GameTicketing";
			case OpStartGame: return L"StartGame";
			case OpLeaveGame: return L"LeaveGame";
			case OpCloseGame: return L"CloseGame";
			case OpCreateCharacters: return L"CreateCharacters";
			case OpUpdateRoom: return L"UpdateRoom";
			case OpCheckGuardian: return L"CheckGuardian";
			case OpCheckDead: return L"CheckDead";
			case OpRpc: return L"Rpc";
			case OpSendRpc: return L"SendRpc";
			case OpDisconnect: return L"Disconnect";
			case OpEndWorkers: return L"EndWorkers";
			default: return L"Unknown";
		}
	}
}
