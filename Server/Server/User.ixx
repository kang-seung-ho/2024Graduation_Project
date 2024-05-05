module;
#include <cstdint>

export module Iconer.Application.User;
import Iconer.Utility.Constraints;
import Iconer.Utility.MovableAtomic;
import Iconer.Net.Socket;
import Iconer.Application.IContext;
import Iconer.Application.ISession;
import Iconer.Application.RoomContract;
import Iconer.Application.GameContract;
import Iconer.Application.BlobSendContext;
import Iconer.Application.BorrowedSendContext;
import Iconer.Application.TransformUnit;
import Iconer.Application.Rpc;
import <tuple>;
import <string>;
import <string_view>;
import <span>;
export import <memory>;

export namespace iconer::app
{
	enum class Team
	{
		Unknown = 0, Red = 1, Blue = 2
	};

	enum class [[nodiscard]] UserStates
	{
		None
		, Reserved
		, Connected, PendingID, Idle
		, InLobby, InRoom, EnteringRoom, LeavingRoom
		, ReadyForGame, MakingGame, InGame
		, Dead
	};

	class [[nodiscard]] User : public ISession<UserStates>, public TransformUnit
	{
	public:
		using Super = ISession<UserStates>;
		using Super::IdType;
		using IoResult = iconer::net::Socket::AsyncResult;
		using BlobIoResult = std::pair<IoResult, BlobSendContext*>;
		using BorrowedIoResult = std::pair<IoResult, Borrower>;

		static inline constexpr bool nothrowDtor = nothrow_destructibles<Super, iconer::net::Socket>;
		static inline constexpr size_t nicknameLength = 16;

		explicit User() = default;

		template<typename ForwardedId>
		[[nodiscard]]
		explicit User(ForwardedId&& id, iconer::net::Socket&& socket)
			noexcept(nothrow_constructible<Super, ForwardedId> and nothrow_move_constructibles<iconer::net::Socket>)
			: Super(std::forward<ForwardedId>(id))
			, TransformUnit()
			, mySocket(std::exchange(socket, iconer::net::Socket{}))
			, recvOffset(0)
			, roomContext()
			, requestContext(AsyncOperations::OpNotifyRoom)
			, requestMemberContext(AsyncOperations::OpNotifyMember)
			, teamChangerContext(AsyncOperations::OpNotifyTeam)
			, gameContext(AsyncOperations::OpCreateGame), loadingContext(AsyncOperations::OpReadyGame)
			, myRoomId(-1)
			, myHealth(100), myTeamId(id % 2 == 0 ? Team::Red : Team::Blue), myWeaponId(0)
			, isRidingGuardian(false)
			, preSignInPacket(), preRoomCreationPacket()
		{
		}

		~User() noexcept(nothrowDtor);

		void Awake();
		void Cleanup() noexcept;

		bool BeginClose() noexcept;
		bool BeginClose(UserStates prev_state) noexcept;
		bool EndClose(UserStates prev_state = UserStates::Dead) noexcept;

		template<size_t Size>
		[[nodiscard]]
		IoResult Receive(std::span<std::byte, Size> buffer)
		{
			if constexpr (Size == std::dynamic_extent)
			{
				return mySocket.Receive(*this, buffer.subspan(recvOffset), buffer.size() - recvOffset);
			}
			else
			{
				return mySocket.Receive(*this, buffer.subspan(recvOffset), Size - recvOffset);
			}
		}

		IoResult SendGeneralData(IContext* ctx, const std::byte* static_buffer, size_t size) noexcept;
		IoResult SendGeneralData(IContext* ctx, const std::byte* static_buffer, size_t size) const noexcept;
		BorrowedIoResult SendGeneralData(const std::byte* static_buffer, size_t size) const;
		BorrowedIoResult SendGeneralData(std::unique_ptr<std::byte[]>&& buffer, size_t size) const;

		IoResult SendSignInPacket() noexcept;
		BorrowedIoResult SendRespondVersionPacket() const;
		BorrowedIoResult SendRespondRoomsPacket(std::span<const std::byte> buffer) const;
		BorrowedIoResult SendRespondMembersPacket(std::span<const std::byte> buffer) const;
		IoResult SendRoomCreatedPacket(IContext* room, IdType room_id) const;
		BorrowedIoResult SendRoomCreationFailedPacket(RoomContract reason) const;
		/// <param name="who">- Not only local client</param>
		BorrowedIoResult SendRoomJoinedPacket(IdType room_id, const User& newbie) const;
		BorrowedIoResult SendRoomJoinFailedPacket(RoomContract reason) const;
		/// <param name="who">- Not only local client</param>
		BorrowedIoResult SendRoomLeftPacket(IdType who, bool is_self) const;
		BorrowedIoResult SendCannotStartGamePacket(GameContract reason) const;
		BorrowedIoResult SendChangeTeamPacket(IdType user_id, bool is_red_team) const;
		BorrowedIoResult SendMakeGameReadyPacket() const;
		BorrowedIoResult SendGameJustStartedPacket() const;
		BorrowedIoResult SendPositionPacket(IdType id, float x, float y, float z) const;
		BorrowedIoResult SendRotationPacket(IdType id, float r, float y, float p) const;
		BorrowedIoResult SendCreateCharactersPacket() const;
		BorrowedIoResult SendRpcPacket(IdType id, RpcProtocol cat, std::int64_t arg0, std::int32_t arg1) const;
		BorrowedIoResult SendOldRpcPacket(IdType id, std::string&& script, long long arg) const;
		BorrowedIoResult SendOldRpcPacket(IdType id, std::string_view script, long long arg) const;

		[[nodiscard]]
		bool IsOnline() const noexcept
		{
			return std::to_underlying(iconer::app::UserStates::Idle) <= std::to_underlying(GetState());
		}

		User(User&&) noexcept = default;
		User& operator=(User&&) noexcept = default;

		iconer::net::Socket mySocket;
		volatile ptrdiff_t recvOffset;

		IContext roomContext;
		IContext requestContext, requestMemberContext;
		IContext teamChangerContext;
		IContext gameContext, loadingContext;

		iconer::util::MovableAtomic<float> myHealth;
		iconer::util::MovableAtomic<IdType> myRoomId;
		iconer::util::MovableAtomic<Team> myTeamId;
		iconer::util::MovableAtomic<std::uint8_t> myWeaponId;
		iconer::util::MovableAtomic<bool> isRidingGuardian;

		std::unique_ptr<std::byte[]> preSignInPacket;
		std::unique_ptr<std::byte[]> preRoomCreationPacket;

	private:
		User(const User&) = delete;
		User& operator=(const User&) = delete;
	};
}
