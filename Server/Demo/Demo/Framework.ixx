module;
#include <vector>
#include <array>
#include <thread>
#include <stop_token>
#include <latch>
#include <string>
#include <string_view>
#include <span>

export module Demo.Framework;
import Iconer.Utility.Logger;
import Iconer.Net.ErrorCode;
import Iconer.Net.IoContext;
import Iconer.Net.Socket;
import Iconer.Net.IoCompletionPort;
import Iconer.Application.IContext;
import Iconer.Application.User;
import Iconer.Application.Room;
import Iconer.Application.ISessionManager;
export import <memory>;
export import <expected>;

export namespace demo
{
	using Buffer = std::unique_ptr<std::byte[]>;

	class Framework
	{
	public:
		using IdType = iconer::app::User::IdType;
		using AcceptResult = iconer::net::Socket::ActionResult;
		using IoResult = iconer::net::Socket::AsyncResult;

		static inline constexpr std::uint16_t lobbySidePort{ 40000 };
		static inline constexpr std::uint16_t gameSidePort{ 40001 };
		static inline constexpr size_t maxUsersNumber = 8000;
		static inline constexpr size_t maxUsersInRoomNumber = 4;
		static inline constexpr size_t maxRoomsNumber = 8000;
		static inline constexpr IdType lobbyServerID = 0;
		static inline constexpr IdType gameServerID = 1;
		static inline constexpr IdType beginUserID = 2;
		static inline constexpr IdType beginRoomID = beginUserID + static_cast<IdType>(maxUsersNumber);
		static inline constexpr size_t userRecvSize = 512;
		static inline constexpr size_t workersCount = 6;

		Framework() = default;
		~Framework() = default;

		void Awake();
		void Start();
		void Update();
		void Destroy();
		void CancelWorkers() noexcept;
		void Cleanup();

		/// <summary>On Awake()</summary>
		bool CreateListenerSockets() noexcept;
		/// <summary>On Awake()</summary>
		bool InitializeLobbyListener() noexcept;
		/// <summary>On Awake()</summary>
		bool InitializeGameListener() noexcept;
		/// <summary>On Awake()</summary>
		bool InitializeCompletionPort(iconer::net::ErrorCode& error_code) noexcept;
		/// <summary>On Awake()</summary>
		bool InitializeUsers();
		/// <summary>On Awake()</summary>
		bool InitializeRooms();
		/// <summary>On Awake()</summary>
		void InitializeSendContextPool();
		/// <summary>On Worker()</summary>
		void CacheSendContexts();
		/// <summary>On Start()</summary>
		bool StartAccepts();

		bool RouteEvent(bool is_succeed, const std::uint64_t& io_id, const ptrdiff_t& io_bytes, iconer::app::IContext* ctx);
		AcceptResult OnReserveAccept(iconer::app::User& user);
		[[noreturn]] void OnFailedReservingAccept();
		AcceptResult OnUserDisconnected(iconer::app::User& user);

		IoResult OnUserConnected(iconer::app::User& user);
		void OnFailedUserConnect(iconer::app::User& user);
		IoResult OnUserSignedIn(iconer::app::User& user, const ptrdiff_t& bytes);
		void OnFailedUserSignIn(iconer::app::User& user);
		IoResult OnNotifyUserId(iconer::app::User& user);
		void OnFailedNotifyId(iconer::app::User& user);

		IoResult OnReceived(iconer::app::User& user, const ptrdiff_t& bytes);
		void OnFailedReceive(iconer::app::User& user);

		iconer::app::RoomContract OnReservingRoom(iconer::app::Room& room, iconer::app::User& user);
		void OnFailedToReserveRoom(iconer::app::Room& room, iconer::app::User& user, iconer::app::RoomContract reason);
		iconer::app::RoomContract OnCreatingRoom(iconer::app::Room& room, iconer::app::User& user);
		void OnFailedToCreateRoom(iconer::app::Room& room, iconer::app::User& user, iconer::app::RoomContract reason);
		iconer::app::RoomContract OnJoiningRoom(iconer::app::Room& room, iconer::app::User& user);
		void OnFailedToJoinRoom(iconer::app::Room& room, iconer::app::User& user, iconer::app::RoomContract reason);
		void OnSucceedToJoinRoom(iconer::app::Room& room, iconer::app::User& user);
		bool OnLeavingRoom(iconer::app::User& user);
		void OnClosingRoom(iconer::app::Room& room);
		IoResult OnNotifyRoomsList(iconer::app::User& user);
		IoResult OnNotifyMemberOfRoom(iconer::app::User& user) noexcept;
		bool OnNotifyTeam(iconer::app::User& user);
		void OnFailedNotifyRoomMember(iconer::app::User& user) noexcept;

		bool OnCreateGame(iconer::app::User& user);
		void OnFailedToCreateGame(iconer::app::User& user) noexcept;

		bool OnBroadcastGameTickets(iconer::app::Room& room);
		void OnFailedToBroadcastGameTickets(iconer::app::Room& room) noexcept;

		bool OnSentGameTicket(iconer::app::User& user);
		void OnFailedToSendGameTicket(iconer::app::User& user) noexcept;

		bool OnGameIsLoaded(iconer::app::User& user);
		void OnFailedToLoadGame(iconer::app::User& user) noexcept;

		bool OnStartGame(iconer::app::Room& room);
		void OnFailedToStartGame(iconer::app::Room& room) noexcept;

		void OnCloseGame(iconer::app::Room& room);

		bool OnCreatingCharacters(iconer::app::Room& room);
		void OnFailedToCreateCharacters(iconer::app::Room& room) noexcept;

		bool OnRpc(iconer::app::IContext* ctx, const IdType& user_id);
		void OnSentRpc(iconer::app::IContext* ctx);
		void OnCleanRpc(iconer::app::IContext* ctx);

		bool OnUpdatingRoom(iconer::app::Room& room);
		void OnFailedToUpdateRoom(iconer::app::Room& room) noexcept;

		void SetRoomModifiedFlag() noexcept;
		bool GetRoomModifiedFlag() const noexcept;

		[[nodiscard]] iconer::app::Borrower AcquireSendContext();
		[[nodiscard]] iconer::app::Borrower AcquireSendContext(Buffer&& buffer, size_t size);

		bool Schedule(iconer::net::IoContext& context, const std::int32_t id, unsigned long info_bytes = 0) noexcept;
		bool Schedule(iconer::net::IoContext* const context, const std::int32_t id, unsigned long info_bytes = 0) noexcept;
		bool Schedule(volatile iconer::net::IoContext& context, const std::int32_t id, unsigned long info_bytes = 0) noexcept;
		bool Schedule(volatile iconer::net::IoContext* const context, const std::int32_t id, unsigned long info_bytes = 0) noexcept;

		[[nodiscard]] iconer::app::User* FindUser(const std::int32_t& id) const noexcept;
		[[nodiscard]] iconer::app::Room* FindRoom(const std::int32_t& id) const noexcept;

		[[nodiscard]] std::span<std::byte, userRecvSize> GetBuffer(const std::int32_t& id) const noexcept;

		[[nodiscard]]
		iconer::net::IoEvent WaitForIoResult() noexcept;
		[[nodiscard]]
		bool IsWorkerCancelled() const noexcept;

		iconer::app::ISessionManager<iconer::app::User>* userManager;
		std::array<iconer::app::Room*, Framework::maxRoomsNumber> everyRoom;
		std::array<std::atomic<iconer::app::RpcContext*>, 30000> everyRpcContexts;
		std::array<std::atomic<iconer::app::RpcContext*>, 30000>::iterator lastRpcIterator;
		std::atomic_bool lastRpcLock;

		std::unique_ptr<iconer::app::User[]> userSpace;
		Buffer recvSpace;
		Buffer serializedRoomsBuffer;
		size_t serializedRoomsBufferSize;

	private:
		friend void Worker(Framework& framework, size_t nth);
		static void LockPhase() noexcept;
		static void UnlockPhase() noexcept;

		iconer::util::Logger myLogger;
		std::vector<std::jthread> serverWorkers;
		std::latch workerAwakens{ workersCount };
		std::stop_source workerCanceller;

		iconer::net::Socket serverListener;
		iconer::net::Socket gameListener;
		iconer::net::IoCompletionPort ioCompletionPort;

		std::atomic_bool haveRoomUpdated;
	};

	constexpr std::int32_t MakeUidToIndex(const std::int32_t& id) noexcept
	{
		return id - Framework::beginUserID;
	}

	constexpr std::int32_t MakeUidToIndex(std::int32_t&& id) noexcept
	{
		return std::move(id) - Framework::beginUserID;
	}

	void Worker(Framework& framework, size_t nth);
}
