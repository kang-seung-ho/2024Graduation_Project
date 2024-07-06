export module Iconer.App.User;
import Iconer.Utility.ReadOnly;
import Iconer.Utility.Delegate;
import Iconer.Net.ErrorCode;
import Iconer.Net.IoResult;
import Iconer.Net.TcpReceiver;
import Iconer.App.UserContext;
import Iconer.App.TaskContext;
import Iconer.App.ISession;
import Iconer.App.ConnectionContract;
import Iconer.App.RoomContract;
import Iconer.App.Settings;
import <memory>;
import <array>;
import <string>;
import <span>;
import <atomic>;

export namespace iconer::app
{
	class [[nodiscard]] Room;
	class [[nodiscard]] SendContext;

	class [[nodiscard]] User : public ISession
	{
	private:
		//static inline constinit std::atomic<id_type> globalMemberTable[Settings::usersLimit * Settings::roomMembersLimit]{};

		static inline constexpr size_t recvBufferSize = 512;

		std::atomic_bool isConnected{};

	public:
		using super = ISession;
		using this_class = User;
		using id_type = super::id_type;

		//userRoomTable[std::array{ 0ull, 1ull }];
		//static inline constinit std::mdspan userRoomTable{ globalMemberTable, std::extents<size_t, Settings::usersLimit, Settings::roomMembersLimit>{} };

		std::wstring myName{};
		std::atomic<Room*> myRoom{};

		iconer::util::ReadOnly<UserContext> mainContext;
		iconer::util::ReadOnly<TaskContext> recvContext{};
		iconer::util::ReadOnly<TaskContext> roomContext{};
		iconer::net::TcpReceiver myReceiver;

		iconer::util::Delegate<void, User*> onDisconnected{};

		explicit constexpr User(id_type id, iconer::net::Socket& socket) noexcept
			: super(id)
			, mainContext(std::in_place, id, this)
			, myReceiver(socket, recvBufferSize)
		{}

		void Cleanup();

		iconer::net::IoResult SendSignInPacket();
		iconer::net::IoResult SendFailedSignInPacket(iconer::app::ConnectionContract reason);
		iconer::net::IoResult SendRoomCreatedPacket(id_type room_id);
		iconer::net::IoResult SendFailedCreateRoomPacket(iconer::app::SendContext& ctx, iconer::app::RoomContract reason);
		iconer::net::IoResult SendJoinedRoomPacket(id_type room_id, id_type user_id);
		iconer::net::IoResult SendOtherJoinedRoomPacket(id_type room_id, id_type user_id);
		iconer::net::IoResult SendRoomJoinFailedPacket(iconer::app::RoomContract reason);

		[[nodiscard]] iconer::net::IoResult BeginOptainReceiveMemory();
		bool EndOptainReceiveMemory(bool flag) noexcept;

		[[nodiscard]]
		iconer::net::IoResult BeginReceive()
		{
			return myReceiver.BeginReceive(recvContext);
		}

		bool EndReceive(const std::uint32_t bytes) noexcept
		{
			return myReceiver.EndReceive(recvContext, bytes);
		}

		iconer::net::IoResult BeginClose();
		void EndClose();

		void SetConnected(bool flag) volatile noexcept
		{
			isConnected.store(flag, std::memory_order_release);
		}

		[[nodiscard]]
		std::span<const std::byte> GetReceivedData() const noexcept
		{
			return myReceiver.GetReceiveBuffer();
		}

		[[nodiscard]]
		std::unique_ptr<std::byte[]> AcquireReceivedData(size_t size)
		{
			return myReceiver.AcquireReceivedData(size);
		}

		[[nodiscard]]
		constexpr const iconer::net::Socket& GetSocket() const noexcept
		{
			return myReceiver.GetSocket();
		}

		[[nodiscard]]
		bool IsConnected() const volatile noexcept
		{
			return isConnected.load(std::memory_order_acquire);
		}

		[[nodiscard]]
		friend constexpr bool operator==(const User& lhs, const User& rhs) noexcept
		{
			return lhs.GetID() == rhs.GetID();
		}
	};
}
