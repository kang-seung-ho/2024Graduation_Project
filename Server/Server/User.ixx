module;
#include <cstdint>
#include <initializer_list>
#include <memory>
#include "glm/glm.hpp"
#include "glm/vec3.hpp"

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
import <span>;

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

	class [[nodiscard]] User : public ISession<UserStates>
	{
	public:
		using Super = ISession<UserStates>;
		using Super::IdType;
		using IoResult = iconer::net::Socket::AsyncResult;
		using BlobIoResult = std::pair<IoResult, BlobSendContext*>;
		using BorrowedIoResult = std::pair<IoResult, Borrower>;

		static inline constexpr size_t nicknameLength = 16;

		explicit User() = default;

		template<typename ForwardedId>
		[[nodiscard]]
		explicit User(ForwardedId&& id, iconer::net::Socket&& socket)
			noexcept(nothrow_constructible<Super, ForwardedId> and nothrow_move_constructibles<iconer::net::Socket>)
			: Super(std::forward<ForwardedId>(id))
			, mySocket(std::exchange(socket, iconer::net::Socket{}))
			, recvOffset(0)
			, roomContext()
			, requestContext(AsyncOperations::OpNotifyRoom)
			, requestMemberContext(AsyncOperations::OpNotifyMember)
			, teamChangerContext(AsyncOperations::OpNotifyTeam)
			, gameContext(AsyncOperations::OpCreateGame), loadingContext(AsyncOperations::OpReadyGame)
			, myTransform()
			, myRoomId(-1), myTeamId(id % 2 == 0 ? Team::Red : Team::Blue), myWeaponId(0)
			, isRidingGuardian(false)
			, preSignInPacket(), preRoomCreationPacket()
		{
		}

		~User() noexcept(nothrow_destructibles<Super, iconer::net::Socket>)
		{
			if (mySocket.IsAvailable())
			{
				std::exchange(mySocket, iconer::net::Socket{}).Close();
			}

			Super::~ISession();
		}

		void Awake();

		bool BeginClose() noexcept
		{
			ContextType::Clear();

			SetState(UserStates::Dead);
			SetOperation(AsyncOperations::OpDisconnect);
			return mySocket.CloseAsync(this);
		}

		bool BeginClose(UserStates prev_state) noexcept
		{
			ContextType::Clear();

			if (TryChangeState(prev_state, UserStates::Dead, std::memory_order_acq_rel))
			{
				SetOperation(AsyncOperations::OpDisconnect);
				return mySocket.CloseAsync(this);
			}
			else
			{
				return false;
			}
		}

		bool EndClose() noexcept
		{
			return TryChangeState(UserStates::Dead, UserStates::None, std::memory_order_acq_rel);
		}

		void Cleanup() noexcept
		{
			SetOperation(AsyncOperations::None);
			SetState(UserStates::None);
			ContextType::Clear();
			myName.clear();
			recvOffset = 0;
			myRoomId = -1;
			myTeamId = GetID() % 2 == 0 ? Team::Red : Team::Blue;
			myWeaponId = 0;
			isRidingGuardian = false;
		}

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
		BorrowedIoResult SendPositionPacket(IdType id, float x, float y, float z) const;
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

		constexpr User& PositionX(const float& v) noexcept
		{
			myTransform[3][0] = v;
			return *this;
		}

		constexpr User& PositionX(float&& v) noexcept
		{
			myTransform[3][0] = std::move(v);
			return *this;
		}

		[[nodiscard]]
		constexpr float& PositionX() noexcept
		{
			return myTransform[3][0];
		}

		[[nodiscard]]
		constexpr const float& PositionX() const noexcept
		{
			return myTransform[3][0];
		}

		constexpr User& PositionY(const float& v) noexcept
		{
			myTransform[3][1] = v;
			return *this;
		}

		constexpr User& PositionY(float&& v) noexcept
		{
			myTransform[3][1] = std::move(v);
			return *this;
		}

		[[nodiscard]]
		constexpr float& PositionY() noexcept
		{
			return myTransform[3][1];
		}

		[[nodiscard]]
		constexpr const float& PositionY() const noexcept
		{
			return myTransform[3][1];
		}

		constexpr User& PositionZ(const float& v) noexcept
		{
			myTransform[3][2] = v;
			return *this;
		}

		constexpr User& PositionZ(float&& v) noexcept
		{
			myTransform[3][2] = std::move(v);
			return *this;
		}

		[[nodiscard]]
		constexpr float& PositionZ() noexcept
		{
			return myTransform[3][2];
		}

		[[nodiscard]]
		constexpr const float& PositionZ() const noexcept
		{
			return myTransform[3][2];
		}

		[[nodiscard]]
		constexpr User& Positions(const float& x, const float& y, const float& z) noexcept
		{
			myTransform[3][0] = x;
			myTransform[3][1] = y;
			myTransform[3][2] = z;
			return *this;
		}

		[[nodiscard]]
		constexpr glm::vec4& Positions() noexcept
		{
			return myTransform[3];
		}

		[[nodiscard]]
		constexpr const glm::vec4& Positions() const noexcept
		{
			return myTransform[3];
		}

		constexpr User& RotationLook(const std::initializer_list<float> list)
		{
			assert(list.size() == 3);
			auto it = list.begin();
			myTransform[2][0] = *(it++);
			myTransform[2][1] = *(it++);
			myTransform[2][2] = *(it++);
			return *this;
		}

		constexpr User& RotationLook(const float(&v)[3]) noexcept
		{
			myTransform[2][0] = v[0];
			myTransform[2][1] = v[1];
			myTransform[2][2] = v[2];
			return *this;
		}

		constexpr User& RotationLook(float(&& v)[3]) noexcept
		{
			myTransform[2][0] = std::move(v[0]);
			myTransform[2][1] = std::move(v[1]);
			myTransform[2][2] = std::move(v[2]);
			return *this;
		}

		constexpr User& RotationLook(const std::span<float, 3> v) noexcept
		{
			myTransform[2][0] = v[0];
			myTransform[2][1] = v[1];
			myTransform[2][2] = v[2];
			return *this;
		}

		constexpr User& RotationLook(const glm::vec4& v) noexcept
		{
			myTransform[2][0] = v[0];
			myTransform[2][1] = v[1];
			myTransform[2][2] = v[2];
			myTransform[2][3] = v[3];
			return *this;
		}

		constexpr User& RotationLook(const glm::vec3& v) noexcept
		{
			myTransform[2][0] = v[0];
			myTransform[2][1] = v[1];
			myTransform[2][2] = v[2];
			return *this;
		}

		[[nodiscard]]
		constexpr glm::vec4& RotationLook() noexcept
		{
			return myTransform[2];
		}

		[[nodiscard]]
		constexpr const glm::vec4& RotationLook() const noexcept
		{
			return myTransform[2];
		}

		constexpr User& RotationUp(const std::initializer_list<float> list)
		{
			assert(list.size() == 3);
			auto it = list.begin();
			myTransform[1][0] = *(it++);
			myTransform[1][1] = *(it++);
			myTransform[1][2] = *(it++);
			return *this;
		}

		constexpr User& RotationUp(const float(&v)[3]) noexcept
		{
			myTransform[1][0] = v[0];
			myTransform[1][1] = v[1];
			myTransform[1][2] = v[2];
			return *this;
		}

		constexpr User& RotationUp(float(&& v)[3]) noexcept
		{
			myTransform[1][0] = std::move(v[0]);
			myTransform[1][1] = std::move(v[1]);
			myTransform[1][2] = std::move(v[2]);
			return *this;
		}

		constexpr User& RotationUp(const std::span<float, 3> v) noexcept
		{
			myTransform[1][0] = v[0];
			myTransform[1][1] = v[1];
			myTransform[1][2] = v[2];
			return *this;
		}

		constexpr User& RotationUp(const glm::vec4& v) noexcept
		{
			myTransform[1][0] = v[0];
			myTransform[1][1] = v[1];
			myTransform[1][2] = v[2];
			myTransform[1][3] = v[3];
			return *this;
		}

		constexpr User& RotationUp(const glm::vec3& v) noexcept
		{
			myTransform[1][0] = v[0];
			myTransform[1][1] = v[1];
			myTransform[1][2] = v[2];
			return *this;
		}

		[[nodiscard]]
		constexpr glm::vec4& RotationUp() noexcept
		{
			return myTransform[1];
		}

		[[nodiscard]]
		constexpr const glm::vec4& RotationUp() const noexcept
		{
			return myTransform[1];
		}

		constexpr User& RotationRight(const std::initializer_list<float> list)
		{
			assert(list.size() == 3);
			auto it = list.begin();
			myTransform[0][0] = *(it++);
			myTransform[0][1] = *(it++);
			myTransform[0][2] = *(it++);
			return *this;
		}

		constexpr User& RotationRight(const float(&v)[3]) noexcept
		{
			myTransform[0][0] = v[0];
			myTransform[0][1] = v[1];
			myTransform[0][2] = v[2];
			return *this;
		}

		constexpr User& RotationRight(float(&& v)[3]) noexcept
		{
			myTransform[0][0] = std::move(v[0]);
			myTransform[0][1] = std::move(v[1]);
			myTransform[0][2] = std::move(v[2]);
			return *this;
		}

		constexpr User& RotationRight(const std::span<float, 3> v) noexcept
		{
			myTransform[0][0] = v[0];
			myTransform[0][1] = v[1];
			myTransform[0][2] = v[2];
			return *this;
		}

		constexpr User& RotationRight(const glm::vec4& v) noexcept
		{
			myTransform[0][0] = v[0];
			myTransform[0][1] = v[1];
			myTransform[0][2] = v[2];
			myTransform[0][3] = v[3];
			return *this;
		}

		constexpr User& RotationRight(const glm::vec3& v) noexcept
		{
			myTransform[0][0] = v[0];
			myTransform[0][1] = v[1];
			myTransform[0][2] = v[2];
			return *this;
		}

		[[nodiscard]]
		constexpr glm::vec4& RotationRight() noexcept
		{
			return myTransform[0];
		}

		[[nodiscard]]
		constexpr const glm::vec4& RotationRight() const noexcept
		{
			return myTransform[0];
		}

		User(User&&) noexcept = default;
		User& operator=(User&&) noexcept = default;

		iconer::net::Socket mySocket;
		volatile ptrdiff_t recvOffset;

		IContext roomContext;
		IContext requestContext, requestMemberContext;
		IContext teamChangerContext;
		IContext gameContext, loadingContext;

		glm::mat4 myTransform;
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
