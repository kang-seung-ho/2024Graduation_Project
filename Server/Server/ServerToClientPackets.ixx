module;
#include <cstddef>
#include <memory>
#include <utility>
#include <algorithm>
#include <ranges>
#include <vector>

export module Iconer.Application.Packet:ServerToClientPackets;
import Iconer.Utility.Serializer;
export import Iconer.Application.BasicPacket;
export import Iconer.Application.RoomContract;
export import Iconer.Application.Rpc;

#include "PacketHelper.inl"

export namespace iconer::app::packets::datagrams
{
#pragma pack(push, 1)
	struct SerializedRoom
	{
		static inline constexpr size_t nameLength = 16;

		std::int32_t id;
		wchar_t title[nameLength];
		size_t members;
	};
	struct [[nodiscard]] SerializedMember
	{
		static inline constexpr size_t nameLength = 16;

		std::int32_t id;
		char team_id; // 1: red, 2: blue
		wchar_t nickname[nameLength];
	};
#pragma pack(pop)
}

export namespace iconer::app::packets::inline sc
{
#pragma pack(push, 1)
	/// <summary>
	/// Broadcasted RPC packet for server
	/// </summary>
	/// <param name="clientId">- An id of the sender client</param>
	/// <param name="rpcScript">- A descriptor for rpc msg</param>
	/// <param name="rpcArgument">- Single rpc argument</param>
	/// <remarks>Server would send it to the client</remarks>
	struct [[nodiscard]] SC_RpcPacket : public BasicPacket
	{
		using Super = BasicPacket;

		static inline constexpr size_t msgLength = 12;

		[[nodiscard]]
		static consteval size_t WannabeSize() noexcept
		{
			return Super::MinSize() + sizeof(clientId) + sizeof(rpcScript) + sizeof(rpcArgument);
		}

		[[nodiscard]]
		static consteval ptrdiff_t SignedWannabeSize() noexcept
		{
			return static_cast<ptrdiff_t>(WannabeSize());
		}

		explicit constexpr SC_RpcPacket(std::int32_t id, const char* begin, const char* end, long long arg = 0)
			noexcept
			: Super(PacketProtocol::SC_RPC, SignedWannabeSize())
			, clientId(id), rpcScript()
			, rpcArgument(arg)
		{
			std::copy(begin, end, rpcScript);
		}

		explicit constexpr SC_RpcPacket(std::int32_t id, const char* nts, const size_t length, long long arg = 0)
			noexcept
			: Super(PacketProtocol::SC_RPC, SignedWannabeSize())
			, clientId(id), rpcScript()
			, rpcArgument(arg)
		{
			std::copy_n(nts, std::min(length, msgLength), rpcScript);
		}

		template<size_t Length>
		explicit constexpr SC_RpcPacket(std::int32_t id, const char(&str)[Length], long long arg = 0)
			noexcept
			: Super(PacketProtocol::SC_RPC, SignedWannabeSize())
			, clientId(id), rpcScript()
			, rpcArgument(arg)
		{
			std::copy_n(str, std::min(Length, msgLength), rpcScript);
		}

		template<size_t Length>
		explicit constexpr SC_RpcPacket(std::int32_t id, char(&& str)[Length], long long arg = 0)
			noexcept
			: Super(PacketProtocol::SC_RPC, SignedWannabeSize())
			, clientId(id), rpcScript()
			, rpcArgument(arg)
		{
			std::move(str, str + std::min(Length, msgLength), rpcScript);
		}

		explicit constexpr SC_RpcPacket(std::int32_t id, const std::string& str, long long arg = 0)
			noexcept
			: Super(PacketProtocol::SC_RPC, SignedWannabeSize())
			, clientId(id), rpcScript()
			, rpcArgument(arg)
		{
			auto it = std::begin(rpcScript);
			const auto max_cnt = std::min(str.length(), msgLength);

			for (auto&& [diff, ch] : std::ranges::views::enumerate(str))
			{
				if (max_cnt <= static_cast<size_t>(diff)) break;

				*it = ch;
			}
		}

		explicit constexpr SC_RpcPacket(std::int32_t id, std::string&& str, long long arg = 0)
			noexcept
			: Super(PacketProtocol::SC_RPC, SignedWannabeSize())
			, clientId(id), rpcScript()
			, rpcArgument(arg)
		{
			auto it = std::begin(rpcScript);
			const auto max_cnt = std::min(str.length(), msgLength);

			for (auto&& [diff, ch] : std::ranges::views::enumerate(str))
			{
				if (max_cnt <= static_cast<size_t>(diff)) break;

				*it = std::move(ch);
			}
		}

		[[nodiscard]]
		constexpr std::unique_ptr<std::byte[]> Serialize() const
		{
			return iconer::util::Serializes(myProtocol, mySize, clientId, std::string_view{ rpcScript, msgLength }, rpcArgument);
		}

		constexpr std::byte* Write(std::byte* buffer) const
		{
			return iconer::util::Serialize(iconer::util::Serialize(iconer::util::Serialize(Super::Write(buffer), clientId), std::string_view{ rpcScript, msgLength }), rpcArgument);
		}

		constexpr const std::byte* Read(const std::byte* buffer)
		{
			return iconer::util::Deserialize(iconer::util::Deserialize(iconer::util::Deserialize(Super::Read(buffer), clientId), rpcScript), rpcArgument);
		}

		std::int32_t clientId;
		char rpcScript[msgLength];
		long long rpcArgument;
	};
	/// <summary>
	/// Position packet for server
	/// </summary>
	/// <param name="clientId">- Target player's id</param>
	/// <param name="x"/>
	/// <param name="y"/>
	/// <param name="z"/>
	/// <remarks>Server would broadcast it to clients</remarks>
	struct [[nodiscard]] SC_UpdatePositionPacket : public BasicPacket
	{
		using Super = BasicPacket;

		[[nodiscard]]
		static consteval size_t WannabeSize() noexcept
		{
			return Super::MinSize() + sizeof(int) + sizeof(float) * 3;
		}

		[[nodiscard]]
		static consteval ptrdiff_t SignedWannabeSize() noexcept
		{
			return static_cast<ptrdiff_t>(Super::MinSize() + sizeof(int) + sizeof(float) * 3);
		}

		constexpr SC_UpdatePositionPacket()
			: SC_UpdatePositionPacket(-1, 0, 0, 0)
		{}

		constexpr SC_UpdatePositionPacket(std::int32_t id, float px, float py, float pz) noexcept
			: Super(PacketProtocol::SC_MOVE_CHARACTER, SignedWannabeSize())
			, clientId(id), x(px), y(py), z(pz)
		{}

		[[nodiscard]]
		constexpr auto Serialize() const
		{
			return iconer::util::Serializes(myProtocol, mySize, clientId, x, y, z);
		}

		constexpr std::byte* Write(std::byte* buffer) const
		{
			return iconer::util::Serializes(Super::Write(buffer), clientId, x, y, z);
		}

		constexpr const std::byte* Read(const std::byte* buffer)
		{
			return iconer::util::Deserialize(iconer::util::Deserialize(iconer::util::Deserialize(iconer::util::Deserialize(Super::Read(buffer), clientId), x), y), z);
		}

		std::int32_t clientId;
		float x, y, z;
	};
	/// <summary>
	/// Rotation packet for server
	/// </summary>
	/// <param name="clientId">- Target player's id</param>
	/// <param name="r"/>
	/// <param name="y"/>
	/// <param name="p"/>
	/// <remarks>Server would broadcast it to clients</remarks>
	struct [[nodiscard]] SC_UpdateRotationPacket : public BasicPacket
	{
		using Super = BasicPacket;

		[[nodiscard]]
		static consteval size_t WannabeSize() noexcept
		{
			return Super::MinSize() + sizeof(clientId) + sizeof(float) * 3;
		}

		[[nodiscard]]
		static consteval ptrdiff_t SignedWannabeSize() noexcept
		{
			return static_cast<ptrdiff_t>(Super::MinSize() + sizeof(clientId) + sizeof(float) * 3);
		}

		constexpr SC_UpdateRotationPacket()
			: SC_UpdateRotationPacket(-1, 0, 0, 0)
		{}

		constexpr SC_UpdateRotationPacket(std::int32_t user_id, float pr, float py, float pp) noexcept
			: Super(PacketProtocol::SC_LOOK_CHARACTER, SignedWannabeSize())
			, clientId(user_id), r(pr), y(py), p(pp)
		{}

		[[nodiscard]]
		constexpr auto Serialize() const
		{
			return iconer::util::Serializes(myProtocol, mySize, clientId, r, y, p);
		}

		constexpr std::byte* Write(std::byte* buffer) const
		{
			return iconer::util::Serializes(Super::Write(buffer), clientId, r, y, p);
		}

		constexpr const std::byte* Read(const std::byte* buffer)
		{
			return iconer::util::Deserialize(iconer::util::Deserialize(iconer::util::Deserialize(iconer::util::Deserialize(Super::Read(buffer), clientId), r), y), p);
		}

		std::int32_t clientId;
		float r, y, p;
	};
	/// <summary>
	/// Team setter packet for server
	/// </summary>
	/// <param name="clientId">- An id of the target client</param>
	/// <param name="teamId">Team's id of user</param>
	/// <remarks>Aerver would send it to the client</remarks>
	MAKE_PACKET_2VAR_WITH_DEFAULT(SC_SetTeamPacket, PacketProtocol::SC_SET_TEAM, std::int32_t, clientId, user_id, 0, std::int8_t, teamId, team_id, 0);
	/// <summary>
	/// Getting game ready notification packet for server
	/// </summary>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_EMPTY_PACKET(SC_ReadyForGamePacket, PacketProtocol::SC_GAME_GETTING_READY);
	/// <summary>
	/// Game starting notification packet for server
	/// </summary>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_EMPTY_PACKET(SC_GameStartPacket, PacketProtocol::SC_GAME_START);
	/// <summary>
	/// Failed to start game notification packet for server
	/// </summary>
	/// <param name="errCause">- Cause of the failure</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_PACKET_1VAR(SC_FailedGameStartingPacket, PacketProtocol::SC_FAILED_GAME_START, int, errCause, error);
	/// <summary>
	/// Creating a client packet for server
	/// </summary>
	/// <param name="clientId">- An id of client</param>
	/// <param name="roomId"/>
	/// <remarks>Server would send it to the client</remarks>
	struct [[nodiscard]] SC_RespondVersionPacket : public BasicPacket
	{
		using Super = BasicPacket;

		static inline constexpr size_t versionLength = 10;

		[[nodiscard]]
		static consteval size_t WannabeSize() noexcept
		{
			return Super::MinSize() + sizeof(gameVersion);
		}

		[[nodiscard]]
		static consteval ptrdiff_t SignedWannabeSize() noexcept
		{
			return static_cast<ptrdiff_t>(WannabeSize());
		}

		constexpr SC_RespondVersionPacket() noexcept
			: Super(PacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, gameVersion()
		{}

		explicit constexpr SC_RespondVersionPacket(const wchar_t* begin, const wchar_t* end)
			: Super(PacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, gameVersion()
		{
			std::copy(begin, end, gameVersion);
		}

		explicit constexpr SC_RespondVersionPacket(const wchar_t* nts, const size_t length)
			: Super(PacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, gameVersion()
		{
			std::copy_n(nts, std::min(length, versionLength), gameVersion);
		}

		template<size_t Length>
		explicit constexpr SC_RespondVersionPacket(const wchar_t(&str)[Length])
			: Super(PacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, gameVersion()
		{
			std::copy_n(str, std::min(Length, versionLength), gameVersion);
		}

		template<size_t Length>
		explicit constexpr SC_RespondVersionPacket(wchar_t(&& str)[Length])
			: Super(PacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, gameVersion()
		{
			std::move(str, str + std::min(Length, versionLength), gameVersion);
		}

		[[nodiscard]]
		constexpr auto Serialize() const
		{
			return iconer::util::Serializes(myProtocol, mySize, std::wstring_view{ gameVersion, versionLength });
		}

		constexpr std::byte* Write(std::byte* buffer) const
		{
			return iconer::util::Serialize(Super::Write(buffer), std::wstring_view{ gameVersion, versionLength });
		}

		constexpr const std::byte* Read(const std::byte* buffer)
		{
			return iconer::util::Deserialize(Super::Read(buffer), gameVersion);
		}

		wchar_t gameVersion[versionLength];
	};
	/// <summary>
	/// Show rooms response packet for server
	/// </summary>
	/// <param name="serializedRooms">- Serialized every room</param>
	/// <remarks>Server would send it to the client</remarks>
	struct [[nodiscard]] SC_RespondRoomsPacket : public BasicPacket
	{
		using Super = BasicPacket;

		[[nodiscard]]
		static consteval size_t MaxSize() noexcept
		{
			return Super::MinSize() + sizeof(datagrams::SerializedRoom) * 20 + sizeof(std::vector<datagrams::SerializedRoom>::size_type);
		}

		[[nodiscard]]
		constexpr size_t WannabeSize() const noexcept
		{
			return Super::MinSize() + sizeof(datagrams::SerializedRoom) * serializedRooms.size() + sizeof(std::vector<datagrams::SerializedRoom>::size_type);
		}

		[[nodiscard]]
		constexpr ptrdiff_t SignedWannabeSize() const noexcept
		{
			return static_cast<ptrdiff_t>(WannabeSize());
		}

		constexpr SC_RespondRoomsPacket() noexcept
			: Super(PacketProtocol::SC_RESPOND_ROOMS, static_cast<std::int16_t>(SignedWannabeSize()))
			, serializedRooms()
		{}

		constexpr void AddMember(std::int32_t room_id, std::wstring_view title, size_t members_count)
		{
			datagrams::SerializedRoom room{ room_id };

			auto it = room.title;
			for (auto ch : title)
			{
				*it = ch;

				if (++it == room.title + sizeof(room.title)) break;
			}
			room.members = members_count;

			serializedRooms.emplace_back(std::move(room));
			mySize = static_cast<std::int16_t>(WannabeSize());
		}

		[[nodiscard]]
		constexpr auto Serialize() const
		{
			std::unique_ptr<std::byte[]> buffer = std::make_unique<std::byte[]>(WannabeSize());

			Write(buffer.get());

			return std::move(buffer);
		}

		constexpr std::byte* Write(std::byte* buffer) const
		{
			auto seek = iconer::util::Serialize(iconer::util::Serialize(buffer, myProtocol), static_cast<std::int16_t>(WannabeSize()));

			seek = iconer::util::Serialize(seek, serializedRooms.size());

			for (auto& room : serializedRooms)
			{
				seek = iconer::util::Serialize(seek, room.id);
				seek = iconer::util::Serialize(seek, std::wstring_view{ room.title, room.nameLength });
				seek = iconer::util::Serialize(seek, room.members);
			}

			return seek;
		}

		constexpr const std::byte* Read(const std::byte* buffer)
		{
			auto seek = Super::Read(buffer);

			size_t size{};
			seek = iconer::util::Deserialize(seek, size);

			if (0 < size)
			{
				serializedRooms.clear();
				serializedRooms.resize(size);

				for (auto& room : serializedRooms)
				{
					seek = iconer::util::Deserialize(seek, room.id);
					seek = iconer::util::Deserialize(seek, room.title);
					seek = iconer::util::Deserialize(seek, room.members);
				}
			}

			return seek;
		}

		std::vector<datagrams::SerializedRoom> serializedRooms;
	};
	/// <summary>
	/// Room members response packet for server
	/// </summary>
	/// <param name="serializedMembers">- Serialized members in the room</param>
	/// <remarks>Server would send it to the client</remarks>
	struct [[nodiscard]] SC_RespondMembersPacket : public BasicPacket
	{
		using Super = BasicPacket;

		[[nodiscard]]
		static consteval size_t MaxSize() noexcept
		{
			return Super::MinSize() + sizeof(datagrams::SerializedMember) * 6 + sizeof(std::vector<datagrams::SerializedMember>::size_type);
		}

		[[nodiscard]]
		constexpr size_t WannabeSize() const noexcept
		{
			return Super::MinSize() + sizeof(datagrams::SerializedMember) * serializedMembers.size() + sizeof(std::vector<datagrams::SerializedMember>::size_type);
		}

		[[nodiscard]]
		constexpr ptrdiff_t SignedWannabeSize() const noexcept
		{
			return static_cast<ptrdiff_t>(WannabeSize());
		}

		constexpr SC_RespondMembersPacket() noexcept
			: Super(PacketProtocol::SC_RESPOND_USERS, static_cast<std::int16_t>(SignedWannabeSize()))
			, serializedMembers()
		{}

		constexpr void AddMember(std::int32_t id, std::wstring_view name, bool is_red_team)
		{
			datagrams::SerializedMember member{ id, is_red_team ? 1 : 2 };

			auto it = member.nickname;
			for (auto ch : name)
			{
				*it = ch;

				if (++it == member.nickname + sizeof(member.nickname)) break;
			}

			serializedMembers.emplace_back(std::move(member));
			mySize = static_cast<std::int16_t>(WannabeSize());
		}

		[[nodiscard]]
		constexpr auto Serialize() const
		{
			std::unique_ptr<std::byte[]> buffer = std::make_unique<std::byte[]>(WannabeSize());

			Write(buffer.get());

			return std::move(buffer);
		}

		constexpr std::byte* Write(std::byte* buffer) const
		{
			auto seek = iconer::util::Serialize(iconer::util::Serialize(buffer, myProtocol), static_cast<std::int16_t>(WannabeSize()));

			seek = iconer::util::Serialize(seek, serializedMembers.size());

			for (auto& member : serializedMembers)
			{
				seek = iconer::util::Serialize(seek, member.id);
				seek = iconer::util::Serialize(seek, member.team_id);
				seek = iconer::util::Serialize(seek, std::wstring_view{ member.nickname, member.nameLength });
			}

			return seek;
		}

		constexpr const std::byte* Read(const std::byte* buffer)
		{
			auto seek = Super::Read(buffer);

			size_t size{};
			seek = iconer::util::Deserialize(seek, size);

			if (0 < size)
			{
				serializedMembers.clear();
				serializedMembers.resize(size);

				for (auto& member : serializedMembers)
				{
					seek = iconer::util::Deserialize(seek, member.id);
					seek = iconer::util::Deserialize(seek, member.team_id);
					seek = iconer::util::Deserialize(seek, member.nickname);
				}
			}

			return seek;
		}

		std::vector<datagrams::SerializedMember> serializedMembers;
	};
	/// <summary>
	/// Room created notification packet for server
	/// </summary>
	/// <param name="roomId">- An id of the created room</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_PACKET_1VAR(SC_RoomCreatedPacket, PacketProtocol::SC_ROOM_CREATED, std::int32_t, roomId, room_id);
	/// <summary>
	/// Failed to join to a room packet for server
	/// </summary>
	/// <param name="errCause">Reason of couldn't join to the room</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_PACKET_1VAR(SC_RoomCreationFailedPacket, PacketProtocol::SC_ROOM_CREATE_FAILED, RoomContract, errCause, cause);
	/// <summary>
	/// Joined to a room packet for server
	/// </summary>
	/// <param name="clientId">- An id of client</param>
	/// <param name="roomId">- An id of the room</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_PACKET_2VAR_WITH_DEFAULT(SC_RoomJoinedPacket, PacketProtocol::SC_ROOM_JOINED, std::int32_t, clientId, user_id, -1, std::int32_t, roomId, room_id, -1);
	/// <summary>
	/// Notifying other joined to the current room packet for server
	/// </summary>
	/// <param name="clientId">- An id of the newie client</param>
	/// <param name="memberInfo">- Information of the newie</param>
	/// <remarks>Server would send it to the client</remarks>
	struct [[nodiscard]] SC_RoomOtherJoinedPacket : public BasicPacket
	{
		using Super = BasicPacket;

		[[nodiscard]]
		static consteval size_t WannabeSize() noexcept
		{
			return Super::MinSize() + sizeof(std::int32_t) + sizeof(datagrams::SerializedMember);
		}

		[[nodiscard]]
		static consteval ptrdiff_t SignedWannabeSize() noexcept
		{
			return static_cast<ptrdiff_t>(WannabeSize());
		}

		constexpr SC_RoomOtherJoinedPacket()
			noexcept(std::conjunction_v<std::is_nothrow_default_constructible<std::int32_t>, std::is_nothrow_default_constructible<datagrams::SerializedMember>>)
			: Super((PacketProtocol::SC_ROOM_JOINED), static_cast<std::int16_t>(SignedWannabeSize()))
			, memberInfo(), roomId()
		{}

		constexpr SC_RoomOtherJoinedPacket(const std::int32_t& room_id, const datagrams::SerializedMember& info)
			noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<std::int32_t>, std::is_nothrow_copy_constructible<datagrams::SerializedMember>>)
			: Super((PacketProtocol::SC_ROOM_JOINED), static_cast<std::int16_t>(SignedWannabeSize()))
			, memberInfo(info), roomId(room_id)
		{}

		constexpr SC_RoomOtherJoinedPacket(std::int32_t&& room_id, const datagrams::SerializedMember& info)
			noexcept(std::conjunction_v<std::is_nothrow_move_constructible<std::int32_t>, std::is_nothrow_copy_constructible<datagrams::SerializedMember>>)
			: Super((PacketProtocol::SC_ROOM_JOINED), static_cast<std::int16_t>(SignedWannabeSize()))
			, memberInfo(info), roomId(std::move(room_id))
		{}

		constexpr SC_RoomOtherJoinedPacket(const std::int32_t& room_id, datagrams::SerializedMember&& info)
			noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<std::int32_t>, std::is_nothrow_move_constructible<datagrams::SerializedMember>>)
			: Super((PacketProtocol::SC_ROOM_JOINED), static_cast<std::int16_t>(SignedWannabeSize()))
			, memberInfo(std::move(info)), roomId(room_id)
		{}

		constexpr SC_RoomOtherJoinedPacket(std::int32_t&& room_id, datagrams::SerializedMember&& info)
			noexcept(std::conjunction_v<std::is_nothrow_move_constructible<std::int32_t>, std::is_nothrow_move_constructible<datagrams::SerializedMember>>)
			: Super((PacketProtocol::SC_ROOM_JOINED), static_cast<std::int16_t>(SignedWannabeSize()))
			, memberInfo(std::move(info)), roomId(std::move(room_id))
		{}

		constexpr std::byte* Write(std::byte* buffer) const
		{
			auto seek = iconer::util::Serialize(iconer::util::Serialize(buffer, myProtocol), static_cast<std::int16_t>(WannabeSize()));

			seek = iconer::util::Serialize(seek, memberInfo.id);
			seek = iconer::util::Serialize(seek, memberInfo.team_id);
			seek = iconer::util::Serialize(seek, std::wstring_view{ memberInfo.nickname, memberInfo.nameLength });
			seek = iconer::util::Serialize(seek, roomId);

			return seek;
		}

		constexpr const std::byte* Read(const std::byte* buffer)
		{
			auto seek = Super::Read(buffer);

			seek = iconer::util::Deserialize(seek, memberInfo.id);
			seek = iconer::util::Deserialize(seek, memberInfo.team_id);
			seek = iconer::util::Deserialize(seek, memberInfo.nickname);
			seek = iconer::util::Deserialize(seek, roomId);

			return seek;
		}

		[[nodiscard]]
		constexpr auto Serialize() const
		{
			std::unique_ptr<std::byte[]> buffer = std::make_unique<std::byte[]>(Super::MinSize() + sizeof(std::int32_t) + sizeof(datagrams::SerializedMember));

			Write(buffer.get());

			return std::move(buffer);
		};

		datagrams::SerializedMember memberInfo;
		std::int32_t roomId;
	};
	/// <summary>
	/// Failed to join to a room packet for server
	/// </summary>
	/// <param name="errCause">- Reason of couldn't join to the room</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_PACKET_1VAR(SC_RoomJoinFailedPacket, PacketProtocol::SC_ROOM_JOIN_FAILED, RoomContract, errCause, cause);
	/// <summary>
	/// Room left packet for server
	/// </summary>
	/// <param name="clientId">- An id of client</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_PACKET_1VAR_WITH_DEFAULT(SC_RoomLeftPacket, PacketProtocol::SC_ROOM_LEFT, std::int32_t, clientId, user_id, -1);
	/// <summary>
	/// Creating a client packet for server
	/// </summary>
	/// <param name="clientId">- An id of client</param>
	/// <param name="roomId"/>
	/// <remarks>Server would send it to the client</remarks>
	struct [[nodiscard]] SC_CreatePlayerPacket : public BasicPacket
	{
		using Super = BasicPacket;

		static inline constexpr size_t nickNameLength = 16;

		[[nodiscard]]
		static consteval size_t WannabeSize() noexcept
		{
			return Super::MinSize() + sizeof(clientId) + sizeof(userName);
		}

		[[nodiscard]]
		static consteval ptrdiff_t SignedWannabeSize() noexcept
		{
			return static_cast<ptrdiff_t>(Super::MinSize() + sizeof(clientId) + sizeof(userName));
		}

		constexpr SC_CreatePlayerPacket() noexcept
			: SC_CreatePlayerPacket(-1)
		{}

		constexpr SC_CreatePlayerPacket(std::int32_t id) noexcept
			: Super(PacketProtocol::SC_CREATE_PLAYER, SignedWannabeSize())
			, clientId(id), userName()
		{}

		[[nodiscard]]
		constexpr auto Serialize() const
		{
			return iconer::util::Serializes(myProtocol, mySize, clientId, userName);
		}

		constexpr std::byte* Write(std::byte* buffer) const
		{
			return iconer::util::Serializes(Super::Write(buffer), clientId, userName);
		}

		constexpr const std::byte* Read(const std::byte* buffer)
		{
			return iconer::util::Deserialize(iconer::util::Deserialize(Super::Read(buffer), clientId), userName);
		}

		std::int32_t clientId;
		wchar_t userName[nickNameLength];
	};
	/// <summary>
	/// Remove a certain client packet for server
	/// </summary>
	/// <param name="clientId">- An id of client</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_PACKET_1VAR(SC_DestroyPlayerPacket, PacketProtocol::SC_REMOVE_PLAYER, std::int32_t, clientId, user_id);
	/// <summary>
	/// Assigning ID to client packet for server
	/// </summary>
	/// <param name="clientId">- An id of client</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_PACKET_1VAR_WITH_DEFAULT(SC_SucceedSignInPacket, PacketProtocol::SC_SIGNIN_SUCCESS, std::int32_t, clientId, user_id, -1);
	/// <summary>
	/// Assigning ID to client packet for server
	/// </summary>
	/// <param name="errCause">- Cause of the failure</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_PACKET_1VAR(SC_FailedSignInPacket, PacketProtocol::SC_SIGNIN_FAILURE, int, errCause, cause);
#pragma pack(pop)
}
