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
import Iconer.Utility.ColourfulConsole;
import Iconer.Collection.FixedString;
import Iconer.Net.ErrorCode;
import Iconer.Net.IoContext;
import Iconer.Net.Socket;
import Iconer.Net.IoCompletionPort;
import Iconer.Application.IContext;
import Iconer.Application.User;
import Iconer.Application.Room;
import Iconer.Application.ISessionManager;
import Iconer.Application.RoomContract;
import Iconer.Application.BorrowedSendContext;
export import <memory>;
export import <expected>;

#define ICONER_NODISCARD [[nodiscard]]
#define ICONER_ALIGN alignas(std::hardware_constructive_interference_size)

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
		static inline constexpr size_t maxUsersNumber = 20;
		static inline constexpr size_t maxUsersInRoomNumber = 6;
		static inline constexpr size_t maxRoomsNumber = maxUsersNumber;
		static inline constexpr IdType lobbyServerID = 0;
		static inline constexpr IdType gameServerID = 1;
		static inline constexpr IdType beginUserID = 2;
		static inline constexpr IdType beginRoomID = beginUserID + static_cast<IdType>(maxUsersNumber);
		static inline constexpr size_t userRecvSize = 512;
		static inline constexpr size_t workersCount = 6;
		static inline constexpr iconer::util::cfc::Colour sessionIdColor = iconer::util::cfc::colors::LightGreen;

		Framework() = default;
		~Framework() = default;

		void Awake();
		void Start();
		void Update();
		void Destroy();
		void CancelWorkers() noexcept;
		void Cleanup();

		ICONER_NODISCARD iconer::app::Borrower AcquireSendContext();
		ICONER_NODISCARD iconer::app::Borrower AcquireSendContext(Buffer&& buffer, size_t size);

		ICONER_NODISCARD
			bool
			Schedule(iconer::net::IoContext& context, const IdType id, unsigned long info_bytes = 0)
			noexcept
		{
			return ioCompletionPort.Schedule(context, static_cast<std::uintptr_t>(id), std::move(info_bytes));
		}

		ICONER_NODISCARD
			bool
			Schedule(iconer::net::IoContext* const context, const IdType id, unsigned long info_bytes = 0)
			noexcept
		{
			return ioCompletionPort.Schedule(context, static_cast<std::uintptr_t>(id), std::move(info_bytes));
		}

		ICONER_NODISCARD
			bool
			Schedule(volatile iconer::net::IoContext& context, const IdType id, unsigned long info_bytes = 0)
			noexcept
		{
			return ioCompletionPort.Schedule(context, static_cast<std::uintptr_t>(id), std::move(info_bytes));
		}

		ICONER_NODISCARD
			bool Schedule(volatile iconer::net::IoContext* const context, const IdType id, unsigned long info_bytes = 0) noexcept
		{
			return ioCompletionPort.Schedule(context, static_cast<std::uintptr_t>(id), std::move(info_bytes));
		}

		ICONER_NODISCARD iconer::net::IoEvent WaitForIoResult() noexcept;

		ICONER_NODISCARD
			iconer::app::User* FindUser(const IdType& id) const noexcept
		{
			return userManager->operator[](id);
		}

		ICONER_NODISCARD
			constexpr iconer::app::Room* FindRoom(const IdType& id) const noexcept
		{
			for (auto&& room : everyRoom)
			{
				if (room->GetID() == id)
				{
					return room;
				}
			}

			return nullptr;
		}

		ICONER_NODISCARD
			constexpr std::span<std::byte, userRecvSize>
			GetBuffer(const IdType& id)
			const noexcept
		{
			std::byte* buf = this->recvSpace.get();
			auto* data = buf + static_cast<ptrdiff_t>(MakeUidToIndex(id)) * userRecvSize;

			return std::span<std::byte, userRecvSize>{ data, userRecvSize };
		}

		ICONER_NODISCARD bool IsWorkerCancelled() const noexcept
		{
			return workerCanceller.stop_requested();
		}

		ICONER_NODISCARD static constexpr IdType MakeUidToIndex(const IdType& id) noexcept
		{
			return id - beginUserID;
		}

		ICONER_NODISCARD static constexpr IdType MakeUidToIndex(IdType&& id) noexcept
		{
			return std::move(id) - beginUserID;
		}

		/// <summary>On Awake()</summary>
		ICONER_NODISCARD bool CreateListenerSockets() noexcept;
		/// <summary>On Awake()</summary>
		ICONER_NODISCARD bool InitializeLobbyListener() noexcept;
		/// <summary>On Awake()</summary>
		ICONER_NODISCARD bool InitializeGameListener() noexcept;
		/// <summary>On Awake()</summary>
		ICONER_NODISCARD bool InitializeCompletionPort(iconer::net::ErrorCode& error_code) noexcept;
		/// <summary>On Awake()</summary>
		ICONER_NODISCARD bool InitializeUsers();
		/// <summary>On Awake()</summary>
		ICONER_NODISCARD bool InitializeRooms();
		/// <summary>On Awake()</summary>
		void InitializeSendContextPool();
		/// <summary>On Worker()</summary>
		void CacheSendContexts();
		/// <summary>On Start()</summary>
		ICONER_NODISCARD bool StartAccepts();

		ICONER_NODISCARD bool RouteEvent(bool is_succeed, const std::uint64_t& io_id, const ptrdiff_t& io_bytes, iconer::app::IContext* ctx);
		ICONER_NODISCARD AcceptResult OnReserveAccept(iconer::app::User& user);
		[[noreturn]] void OnFailedReservingAccept();
		ICONER_NODISCARD AcceptResult OnUserDisconnected(iconer::app::User& user);

		ICONER_NODISCARD IoResult OnUserConnected(iconer::app::User& user);
		void OnFailedUserConnect(iconer::app::User& user);
		ICONER_NODISCARD IoResult OnUserSignedIn(iconer::app::User& user, const ptrdiff_t& bytes);
		void OnFailedUserSignIn(iconer::app::User& user);
		ICONER_NODISCARD IoResult OnNotifyUserId(iconer::app::User& user);
		void OnFailedNotifyId(iconer::app::User& user);

		ICONER_NODISCARD IoResult OnReceived(iconer::app::User& user, const ptrdiff_t& bytes);
		void OnFailedReceive(iconer::app::User& user);

		ICONER_NODISCARD iconer::app::RoomContract OnReservingRoom(iconer::app::Room& room, iconer::app::User& user);
		void OnFailedToReserveRoom(iconer::app::Room& room, iconer::app::User& user, iconer::app::RoomContract reason);
		ICONER_NODISCARD iconer::app::RoomContract OnCreatingRoom(iconer::app::Room& room, iconer::app::User& user);
		void OnFailedToCreateRoom(iconer::app::Room& room, iconer::app::User& user, iconer::app::RoomContract reason);
		ICONER_NODISCARD iconer::app::RoomContract OnJoiningRoom(iconer::app::Room& room, iconer::app::User& user);
		void OnFailedToJoinRoom(iconer::app::Room& room, iconer::app::User& user, iconer::app::RoomContract reason);
		void OnSucceedToJoinRoom(iconer::app::Room& room, iconer::app::User& user);
		bool OnLeavingRoom(iconer::app::User& user);
		void OnClosingRoom(iconer::app::Room& room);
		ICONER_NODISCARD IoResult OnNotifyRoomsList(iconer::app::User& user);
		ICONER_NODISCARD IoResult OnNotifyMemberOfRoom(iconer::app::User& user) noexcept;
		ICONER_NODISCARD bool OnNotifyTeam(iconer::app::User& user);
		void OnFailedNotifyRoomMember(iconer::app::User& user) noexcept;

		ICONER_NODISCARD bool OnCreateGame(iconer::app::User& user);
		void OnFailedToCreateGame(iconer::app::User& user) noexcept;

		ICONER_NODISCARD bool OnBroadcastGameTickets(iconer::app::Room& room);
		void OnFailedToBroadcastGameTickets(iconer::app::Room& room) noexcept;

		ICONER_NODISCARD bool OnSentGameTicket(iconer::app::User& user);
		void OnFailedToSendGameTicket(iconer::app::User& user) noexcept;

		ICONER_NODISCARD bool OnGameIsLoaded(iconer::app::User& user);
		void OnFailedToLoadGame(iconer::app::User& user) noexcept;

		ICONER_NODISCARD bool OnStartGame(iconer::app::Room& room);
		void OnFailedToStartGame(iconer::app::Room& room) noexcept;

		void OnCloseGame(iconer::app::Room& room);

		ICONER_NODISCARD bool OnCreatingCharacters(iconer::app::Room& room);
		void OnFailedToCreateCharacters(iconer::app::Room& room) noexcept;

		ICONER_NODISCARD bool OnRpc(iconer::app::IContext* ctx, const IdType& user_id);
		void OnSentRpc(iconer::app::IContext* ctx);
		void OnCleanRpc(iconer::app::IContext* ctx);

		void SetRoomModifiedFlag() noexcept;
		ICONER_NODISCARD bool GetRoomModifiedFlag() const noexcept;

		ICONER_ALIGN iconer::app::ISessionManager<iconer::app::User>* userManager;
		ICONER_ALIGN std::array<iconer::app::Room*, Framework::maxRoomsNumber> everyRoom;
		ICONER_ALIGN std::array<std::atomic<iconer::app::RpcContext*>, 10000> everyRpcContexts;
		ICONER_ALIGN std::array<std::atomic<iconer::app::RpcContext*>, 10000>::iterator lastRpcIterator;
		ICONER_ALIGN std::atomic_bool lastRpcLock;

		ICONER_ALIGN std::unique_ptr<iconer::app::User[]> userSpace;
		ICONER_ALIGN Buffer recvSpace;
		ICONER_ALIGN Buffer serializedRoomsBuffer;
		ICONER_ALIGN size_t serializedRoomsBufferSize;

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

	void Worker(Framework& framework, size_t nth);
}

iconer::net::IoEvent
demo::Framework::WaitForIoResult()
noexcept
{
	return ioCompletionPort.WaitForIoResult();
}
