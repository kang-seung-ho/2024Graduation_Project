module;
#include <cstddef>
#include <utility>
#include <algorithm>

export module Iconer.Application.Packet:ClientToServerPackets;
import Iconer.Utility.Serializer;
export import Iconer.Application.BasicPacket;
export import Iconer.Application.RoomContract;

#include "PacketHelper.inl"

export namespace iconer::app::packets::inline cs
{
#pragma pack(push, 1)
	/// <summary>
	/// RPC packet for client
	/// </summary>
	/// <param name="rpcScript">- A descriptor for rpc msg</param>
	/// <remarks>Client would send it to the server</remarks>
	struct [[nodiscard]] CS_RpcPacket : public BasicPacket
	{
		using Super = BasicPacket;

		static inline constexpr size_t msgLength = 12;

		[[nodiscard]]
		static consteval size_t WannabeSize() noexcept
		{
			return Super::MinSize() + sizeof(rpcScript) + sizeof(rpcArgument);
		}

		[[nodiscard]]
		static consteval ptrdiff_t SignedWannabeSize() noexcept
		{
			return static_cast<ptrdiff_t>(WannabeSize());
		}

		[[nodiscard]]
		constexpr std::unique_ptr<std::byte[]> Serialize() const
		{
			return iconer::util::Serializes(myProtocol, mySize, std::string_view{ rpcScript, msgLength }, rpcArgument);
		}

		constexpr std::byte* Write(std::byte* buffer) const
		{
			return iconer::util::Serialize(iconer::util::Serialize(Super::Write(buffer), std::string_view{ rpcScript, msgLength }), rpcArgument);
		}

		constexpr const std::byte* Read(const std::byte* buffer)
		{
			return iconer::util::Deserialize(iconer::util::Deserialize(Super::Read(buffer), msgLength, rpcScript), rpcArgument);
		}

		char rpcScript[msgLength];
		long long rpcArgument;
	};
	/// <summary>
	/// Team setter packet for client
	/// </summary>
	/// <param name="teamId">Team's id of user</param>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET_1VAR_WITH_DEFAULT(CS_SetTeamPacket, PacketProtocol::CS_SET_TEAM, std::int8_t, teamId, team_id, 0);
	/// <summary>
	/// Requesting game version packet for client
	/// </summary>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET(CS_RequestVersionPacket, PacketProtocol::CS_REQUEST_VERSION);
	/// <summary>
	/// Requesting every room packet for client
	/// </summary>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET(CS_RequestRoomsPacket, PacketProtocol::CS_REQUEST_ROOMS);
	/// <summary>
	/// Requesting users in the room packet for client
	/// </summary>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET(CS_RequestMembersPacket, PacketProtocol::CS_REQUEST_USERS);
	/// <summary>
	/// Game initiating packet for client
	/// </summary>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET(CS_GameStartPacket, PacketProtocol::CS_GAME_START);
	/// <summary>
	/// Notifying game's loading is done packet for client
	/// </summary>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET(CS_GameLoadedPacket, PacketProtocol::CS_GAME_LOADED);
	/// <summary>
	/// Room create packet for client
	/// </summary>
	/// <param name="roomTitle">Title of the new room</param>
	/// <remarks>Client would send it to the server</remarks>
	struct [[nodiscard]] CS_CreateRoomPacket : public BasicPacket
	{
		using Super = BasicPacket;

		static inline constexpr size_t roomTitleLength = 16;

		[[nodiscard]]
		static consteval size_t WannabeSize() noexcept
		{
			return Super::MinSize() + sizeof(roomTitle);
		}

		[[nodiscard]]
		static consteval ptrdiff_t SignedWannabeSize() noexcept
		{
			return static_cast<ptrdiff_t>(Super::MinSize() + sizeof(roomTitle));
		}

		constexpr CS_CreateRoomPacket() noexcept
			: Super(PacketProtocol::CS_ROOM_CREATE, SignedWannabeSize())
			, roomTitle()
		{
		}

		explicit constexpr CS_CreateRoomPacket(const wchar_t* begin, const wchar_t* end)
			: Super(PacketProtocol::CS_ROOM_CREATE, SignedWannabeSize())
			, roomTitle()
		{
			std::copy(begin, end, roomTitle);
		}

		explicit constexpr CS_CreateRoomPacket(const wchar_t* nts, const size_t length)
			: Super(PacketProtocol::CS_ROOM_CREATE, SignedWannabeSize())
			, roomTitle()
		{
			std::copy_n(nts, std::min(length, roomTitleLength), roomTitle);
		}

		template<size_t Length>
		explicit constexpr CS_CreateRoomPacket(const wchar_t(&str)[Length])
			: Super(PacketProtocol::CS_ROOM_CREATE, SignedWannabeSize())
			, roomTitle()
		{
			std::copy_n(str, std::min(Length, roomTitleLength), roomTitle);
		}

		template<size_t Length>
		explicit constexpr CS_CreateRoomPacket(wchar_t(&& str)[Length])
			: Super(PacketProtocol::CS_ROOM_CREATE, SignedWannabeSize())
			, roomTitle()
		{
			std::move(str, str + std::min(Length, roomTitleLength), roomTitle);
		}

		[[nodiscard]]
		constexpr auto Serialize() const
		{
			return iconer::util::Serializes(myProtocol, mySize, std::wstring_view{ roomTitle, roomTitleLength });
		}

		constexpr std::byte* Write(std::byte* buffer) const
		{
			return iconer::util::Serialize(Super::Write(buffer), std::wstring_view{ roomTitle, roomTitleLength });
		}

		constexpr const std::byte* Read(const std::byte* buffer, const size_t&)
		{
			return iconer::util::Deserialize(Super::Read(buffer), roomTitleLength, roomTitle);
		}

		wchar_t roomTitle[roomTitleLength];
	};
	/// <summary>
	/// Room entering packet for client
	/// </summary>
	/// <param name="roomId"/>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET_1VAR_WITH_DEFAULT(CS_EnterRoomPacket, PacketProtocol::CS_ROOM_JOIN, std::int32_t, roomId, room_id, -1);
	/// <summary>
	/// Room leaving packet for client
	/// </summary>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET(CS_LeaveRoomPacket, PacketProtocol::CS_ROOM_LEAVE);
	/// <summary>
	/// Position packet for client
	/// </summary>
	/// <param name="x"/>
	/// <param name="y"/>
	/// <param name="z"/>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET_3VAR(CS_UpdatePositionPacket, PacketProtocol::CS_MY_POSITION, float, x, px, float, y, py, float, z, pz);
	/// <summary>
	/// Rotation packet for client
	/// </summary>
	/// <param name="r">roll</param>
	/// <param name="y">yaw</param>
	/// <param name="p">pitch</param>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET_3VAR(CS_UpdateRotationPacket, PacketProtocol::CS_MY_TRANSFORM, float, r, pr, float, y, py, float, p, pp);
	/// <summary>
	/// Login packet for client
	/// </summary>
	/// <param name="userName">Nickname of client</param>
	/// <remarks>Client would send it to the server</remarks>
	struct [[nodiscard]] CS_SignInPacket : public BasicPacket
	{
		using Super = BasicPacket;

		static inline constexpr size_t nickNameLength = 16;

		[[nodiscard]]
		static consteval size_t WannabeSize() noexcept
		{
			return Super::MinSize() + sizeof(userName);
		}

		[[nodiscard]]
		static consteval ptrdiff_t SignedWannabeSize() noexcept
		{
			return static_cast<ptrdiff_t>(Super::MinSize() + sizeof(userName));
		}

		constexpr CS_SignInPacket() noexcept
			: Super(PacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, userName()
		{
		}

		explicit constexpr CS_SignInPacket(const wchar_t* begin, const wchar_t* end)
			: Super(PacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, userName()
		{
			std::copy(begin, end, userName);
		}

		explicit constexpr CS_SignInPacket(const wchar_t* nts, const size_t length)
			: Super(PacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, userName()
		{
			std::copy_n(nts, std::min(length, nickNameLength), userName);
		}

		template<size_t Length>
		explicit constexpr CS_SignInPacket(const wchar_t(&str)[Length])
			: Super(PacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, userName()
		{
			std::copy_n(str, std::min(Length, nickNameLength), userName);
		}

		template<size_t Length>
		explicit constexpr CS_SignInPacket(wchar_t(&& str)[Length])
			: Super(PacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, userName()
		{
			std::move(str, str + std::min(Length, nickNameLength), userName);
		}

		[[nodiscard]]
		constexpr auto Serialize() const
		{
			return iconer::util::Serializes(myProtocol, mySize, std::wstring_view{ userName, nickNameLength });
		}

		constexpr std::byte* Write(std::byte* buffer) const
		{
			return iconer::util::Serialize(Super::Write(buffer), std::wstring_view{ userName, nickNameLength });
		}

		constexpr const std::byte* Read(const std::byte* buffer, const size_t& buffer_length)
		{
			return iconer::util::Deserialize(Super::Read(buffer), nickNameLength, userName);
		}

		wchar_t userName[nickNameLength];
	};
#pragma pack(pop)
}
