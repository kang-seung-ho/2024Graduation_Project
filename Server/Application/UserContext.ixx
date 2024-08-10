export module Iconer.App.UserContext;
import Iconer.Utility.ReadOnly;
import iconer.Utility.Serializer;
import Iconer.App.TaskContext;
import Iconer.App.ISession;
import Iconer.App.PacketSerializer;
import Iconer.App.ConnectionContract;
import Iconer.App.RoomContract;
import Iconer.App.RpcProtocols;
import <cstddef>;
import <span>;
import <array>;

export namespace iconer::app
{
	class [[nodiscard]] UserContext : public TaskContext
	{
	public:
		using super = TaskContext;
		using this_class = UserContext;
		using id_type = iconer::app::ISession::id_type;

		static inline constexpr size_t signInPacketSize = 3 + sizeof(std::int32_t);
		static inline constexpr size_t signInFailedPacketSize = 3 + sizeof(std::uint32_t);
		static inline constexpr size_t roomCreatedPacketSize = 3 + sizeof(std::int32_t);
		static inline constexpr size_t roomCreateFailedPacketSize = 3 + sizeof(iconer::app::RoomContract);
		static inline constexpr size_t roomJoinedPacketSize = 3 + sizeof(std::int32_t) * 2 + 32;
		static inline constexpr size_t roomJoinFailedPacketSize = 3 + sizeof(iconer::app::RoomContract);
		static inline constexpr size_t gameReadyPacketSize = 3;
		static inline constexpr size_t roomLeftPacketSize = 3 + sizeof(std::int32_t);

		iconer::util::ReadOnly<id_type> ownerId;
		iconer::util::ReadOnly<class User*> ownerHandle;

		constexpr UserContext(id_type id, class User* ptr)
			: super()
			, ownerId(id), ownerHandle(ptr)
		{
			const auto small_id = static_cast<std::int32_t>(id);

			(void)SerializeAt(signInPacketData, PacketProtocol::SC_SIGNIN_SUCCESS, small_id);

			(void)SerializeAt(signInFailedPacketData, PacketProtocol::SC_SIGNIN_FAILURE, 0U);

			(void)SerializeAt(roomCreatedPacketData, PacketProtocol::SC_ROOM_CREATED, -1);
			(void)SerializeAt(roomCreateFailedPacketData, PacketProtocol::SC_ROOM_CREATE_FAILED, iconer::app::RoomContract::Success);

			(void)SerializeAt(roomJoinedPacketData, PacketProtocol::SC_ROOM_JOINED, small_id, -1);
			(void)SerializeAt(roomJoinFailedPacketData, PacketProtocol::SC_ROOM_JOIN_FAILED, iconer::app::RoomContract::Success);

			(void)SerializeAt(roomJoinFailedPacketData, PacketProtocol::SC_ROOM_JOIN_FAILED, iconer::app::RoomContract::Success);

			(void)SerializeAt(roomLeftPacketData, PacketProtocol::SC_ROOM_LEFT, static_cast<std::int32_t>(static_cast<std::int64_t>(id)));

			iconer::app::SerializeAt(rideGuardianPacketData, PacketProtocol::SC_RPC, small_id, RpcProtocol::RPC_BEG_RIDE, 0LL, 0);
			iconer::app::SerializeAt(doRespawnPacketData, PacketProtocol::SC_RPC, small_id, RpcProtocol::RPC_RESPAWN, 0LL, 0);
			iconer::app::SerializeAt(respawnTimePacketData, PacketProtocol::SC_RPC, small_id, RpcProtocol::RPC_RESPAWN_TIMER, 0LL, 0);
		}

		[[nodiscard]]
		constexpr auto& GetSignInPacketData() const noexcept
		{
			return signInPacketData;
		}

		[[nodiscard]]
		constexpr auto& GetSignInFailurePacketData() const noexcept
		{
			return signInFailedPacketData;
		}

		[[nodiscard]]
		constexpr auto& GetRoomCreatedPacketData(std::int32_t room_id) noexcept
		{
			iconer::util::Serialize(roomCreatedPacketData.data() + 3, room_id);

			return roomCreatedPacketData;
		}

		[[nodiscard]]
		constexpr auto& GetRoomCreateFailedPacketData(iconer::app::RoomContract reason) noexcept
		{
			iconer::util::Serialize(roomCreateFailedPacketData.data() + 3, reason);

			return roomCreateFailedPacketData;
		}

		[[nodiscard]]
		constexpr auto& GetRoomJoinedPacketData(std::int32_t room_id) noexcept
		{
			iconer::util::Serialize(roomJoinedPacketData.data() + 3 + sizeof(std::int32_t), room_id);

			return roomJoinedPacketData;
		}

		[[nodiscard]]
		constexpr auto& GetRoomJoinFailedPacketData(iconer::app::RoomContract reason) noexcept
		{
			iconer::util::Serialize(roomJoinFailedPacketData.data() + 3, reason);

			return roomJoinFailedPacketData;
		}

		[[nodiscard]]
		constexpr auto& GetRoomLeftPacketData() noexcept
		{
			return roomLeftPacketData;
		}

		[[nodiscard]]
		static constexpr auto& GetGameReadyPacketData() noexcept
		{
			return gameReadyPacketData;
		}

		[[nodiscard]]
		static constexpr auto& GetGameStartPacketData() noexcept
		{
			return gameStartPacketData;
		}

		[[nodiscard]]
		static constexpr auto& GetCreatCharactersPacketData() noexcept
		{
			return createCharactersPacketData;
		}

		[[nodiscard]]
		constexpr auto& GetRideGuardianPacketData(const std::int32_t& guardian_id) noexcept
		{
			iconer::util::Serialize(rideGuardianPacketData.data() + rpcPacketHeaderSize + sizeof(std::int64_t), guardian_id);

			return rideGuardianPacketData;
		}

		[[nodiscard]]
		constexpr auto& GetRespawnPacketData() noexcept
		{
			return doRespawnPacketData;
		}

		[[nodiscard]]
		constexpr auto& GetRespawnTimePacketData(const std::int64_t& time_count) noexcept
		{
			iconer::util::Serialize(respawnTimePacketData.data() + rpcPacketHeaderSize + time_count, 0);

			return respawnTimePacketData;
		}

	private:
		std::array<std::byte, signInPacketSize> signInPacketData{};
		std::array<std::byte, signInFailedPacketSize> signInFailedPacketData{};
		std::array<std::byte, roomCreatedPacketSize> roomCreatedPacketData{};
		std::array<std::byte, roomCreateFailedPacketSize> roomCreateFailedPacketData{};
		std::array<std::byte, roomJoinedPacketSize> roomJoinedPacketData{};
		std::array<std::byte, roomJoinFailedPacketSize> roomJoinFailedPacketData{};
		std::array<std::byte, roomLeftPacketSize> roomLeftPacketData{};

		/** RPC packet datas
		* 
		* 137[1] => SC_RPC
		* 20|0[2] => size
		* 0[4] => id
		* [8] => arg0
		* [4] => arg1
		*/
		static inline constexpr size_t rpcPacketHeaderSize = 3 + 4;

		std::array<std::byte, 20> rideGuardianPacketData{};
		std::array<std::byte, 20> doRespawnPacketData{};
		std::array<std::byte, 20> respawnTimePacketData{};

		// 139 => SC_GAME_GETTING_READY
		// 3 => size
		static inline constexpr std::array<std::byte, 3> gameReadyPacketData
		{
			(std::byte)139U, (std::byte)3U, (std::byte)0U
		};
		// 138 => SC_GAME_START
		// 3 => size
		static inline constexpr std::array<std::byte, 3> gameStartPacketData
		{
			(std::byte)138U, (std::byte)3U, (std::byte)0U
		};
		// 143 => SC_CREATE_PLAYER
		// 3 => size
		static inline constexpr std::array<std::byte, 3> createCharactersPacketData
		{
			(std::byte)143U, (std::byte)3U, (std::byte)0U
		};
	};
}
