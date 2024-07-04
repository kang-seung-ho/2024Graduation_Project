export module Iconer.App.UserContext;
import Iconer.Utility.ReadOnly;
import iconer.Utility.Serializer;
import Iconer.App.TaskContext;
import Iconer.App.ISession;
import Iconer.App.PacketSerializer;
import Iconer.App.ConnectionContract;
import Iconer.App.RoomContract;
import <cstddef>;
import <span>;

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

		iconer::util::ReadOnly<id_type> ownerId;
		iconer::util::ReadOnly<class User*> ownerHandle;

		constexpr UserContext(id_type id, class User* ptr)
			: super()
			, ownerId(id), ownerHandle(ptr)
		{
			(void)SerializeAt(signInPacketData, PacketProtocol::SC_SIGNIN_SUCCESS, static_cast<std::int32_t>(id));

			(void)SerializeAt(signInFailedPacketData, PacketProtocol::SC_SIGNIN_FAILURE, 0U);

			(void)SerializeAt(roomCreatedPacketData, PacketProtocol::SC_ROOM_CREATED, -1);

			(void)SerializeAt(roomCreateFailedPacketData, PacketProtocol::SC_ROOM_CREATE_FAILED, iconer::app::RoomContract::Success);
		}

		[[nodiscard]]
		constexpr std::span<const std::byte, signInPacketSize> GetSignInPacketData() const noexcept
		{
			return std::span{ signInPacketData };
		}
		
		[[nodiscard]]
		constexpr std::span<const std::byte, signInFailedPacketSize> GetSignInFailurePacketData() const noexcept
		{
			return std::span{ signInFailedPacketData };
		}
		
		[[nodiscard]]
		constexpr std::span<std::byte, roomCreatedPacketSize> GetRoomCreatedPacketData(std::int32_t room_id) noexcept
		{
			iconer::util::Serialize(roomCreatedPacketData + 3, room_id);

			return std::span{ roomCreatedPacketData };
		}
		
		[[nodiscard]]
		constexpr std::span<std::byte, roomCreateFailedPacketSize> GetRoomCreateFailedPacketData(iconer::app::RoomContract reason) noexcept
		{
			iconer::util::Serialize(roomCreateFailedPacketData + 3, reason);

			return std::span{ roomCreateFailedPacketData };
		}

	private:
		std::byte signInPacketData[signInPacketSize]{};
		std::byte signInFailedPacketData[signInFailedPacketSize]{};
		std::byte roomCreatedPacketData[roomCreatedPacketSize]{};
		std::byte roomCreateFailedPacketData[roomCreateFailedPacketSize]{};
	};
}
