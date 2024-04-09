#pragma once
#ifndef SAGAFRAMEWORK_NET_SC_PACKET_PREFABS_H
#define SAGAFRAMEWORK_NET_SC_PACKET_PREFABS_H

#include "SagaNetwork.h"
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "Saga/Network/SagaBasicPacket.h"
#include "Saga/Network/SagaPacketProtocol.h"
#include "Saga/Network/SagaRoomContract.h"
#include "Saga/Network/SagaPacketHelper.inl"

namespace saga::datagrams
{
	struct SerializedRoom
	{
		static inline constexpr size_t nameLength = 16;

		int32 id;
		wchar_t title[nameLength];
		size_t members;
	};
	struct SerializedMember
	{
		static inline constexpr size_t nameLength = 16;

		int32 id;
		char team_id; // 1: red, 2: blue
		wchar_t nickname[nameLength];
	};
}

namespace saga::inline sc
{
	/// <summary>
	/// Broadcasted RPC packet for server
	/// </summary>
	/// <param name="clientId">- An id of the sender client</param>
	/// <param name="rpcScript">- A descriptor for rpc msg</param>
	/// <param name="rpcArgument">- Single rpc argument</param>
	/// <remarks>Server would send it to the client</remarks>
	struct SC_RpcPacket : public FSagaBasicPacket
	{
		using Super = FSagaBasicPacket;

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

		constexpr SC_RpcPacket()
			noexcept
			: Super(EPacketProtocol::SC_RPC, SignedWannabeSize())
			, clientId(-1), rpcScript()
			, rpcArgument()
		{}

		MAKE_SERIALIZE_METHOD();
		MAKE_RW_METHODS();

		int32 clientId;
		char rpcScript[msgLength];
		long long rpcArgument;
	};
	/// <summary>
	/// Team setter packet for server
	/// </summary>
	/// <param name="teamId">Team's id of user</param>
	/// <remarks>Aerver would send it to the client</remarks>
	MAKE_EMPTY_PACKET_2VAR_WITH_DEFAULT(SC_SetTeamPacket, EPacketProtocol::SC_SET_TEAM, int32, clientId, user_id, 0, std::int8_t, teamId, team_id, 0);
	/// <summary>
	/// Getting game ready notification packet for server
	/// </summary>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_EMPTY_PACKET(SC_ReadyForGamePacket, EPacketProtocol::SC_GAME_GETTING_READY);
	/// <summary>
	/// Game starting notification packet for server
	/// </summary>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_EMPTY_PACKET(SC_GameStartPacket, EPacketProtocol::SC_GAME_START);
	/// <summary>
	/// Failed to start game notification packet for server
	/// </summary>
	/// <param name="errCause">- Cause of the failure</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_EMPTY_PACKET_1VAR(SC_FailedGameStartingPacket, EPacketProtocol::SC_FAILED_GAME_START, int32, errCause, error, true);
	/// <summary>
	/// Creating a client packet for server
	/// </summary>
	/// <param name="clientId">- An id of client</param>
	/// <param name="roomId"/>
	/// <remarks>Server would send it to the client</remarks>
	struct SC_RespondVersionPacket : public FSagaBasicPacket
	{
		using Super = FSagaBasicPacket;

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
			: Super(EPacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, gameVersion()
		{}

		explicit constexpr SC_RespondVersionPacket(const wchar_t* begin, const wchar_t* end)
			: Super(EPacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, gameVersion()
		{
			std::copy(begin, end, gameVersion);
		}

		explicit constexpr SC_RespondVersionPacket(const wchar_t* nts, const size_t length)
			: Super(EPacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, gameVersion()
		{
			std::copy_n(nts, std::min(length, versionLength), gameVersion);
		}

		template<size_t Length>
		explicit constexpr SC_RespondVersionPacket(const wchar_t(&str)[Length])
			: Super(EPacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, gameVersion()
		{
			std::copy_n(str, std::min(Length, versionLength), gameVersion);
		}

		template<size_t Length>
		explicit constexpr SC_RespondVersionPacket(wchar_t(&& str)[Length])
			: Super(EPacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, gameVersion()
		{
			std::move(str, str + std::min(Length, versionLength), gameVersion);
		}

		MAKE_SERIALIZE_METHOD();
		MAKE_RW_METHODS();

		wchar_t gameVersion[versionLength];
	};
	/// <summary>
	/// Show rooms response packet for server
	/// </summary>
	/// <param name="serializedRooms">- Serialized every room</param>
	/// <remarks>Server would send it to the client</remarks>
	struct SC_RespondRoomsPacket : public FSagaBasicPacket
	{
		using Super = FSagaBasicPacket;

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
			: Super(EPacketProtocol::SC_RESPOND_ROOMS, static_cast<int16>(SignedWannabeSize()))
			, serializedRooms()
		{}

		constexpr void AddMember(int32 room_id, std::wstring_view title, size_t members_count)
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
			mySize = static_cast<int16>(WannabeSize());
		}

		MAKE_SERIALIZE_METHOD();
		MAKE_RW_METHODS();

		std::vector<datagrams::SerializedRoom> serializedRooms;
	};
	/// <summary>
	/// Room members response packet for server
	/// </summary>
	/// <param name="serializedMembers">- Serialized members in the room</param>
	/// <remarks>Server would send it to the client</remarks>
	struct SC_RespondMembersPacket : public FSagaBasicPacket
	{
		using Super = FSagaBasicPacket;

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
			: Super(EPacketProtocol::SC_RESPOND_USERS, static_cast<int16>(SignedWannabeSize()))
			, serializedMembers()
		{}

		MAKE_SERIALIZE_METHOD();
		MAKE_RW_METHODS();

		std::vector<datagrams::SerializedMember> serializedMembers;
	};
	/// <summary>
	/// Room created notification packet for server
	/// </summary>
	/// <param name="roomId">- An id of the created room</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_EMPTY_PACKET_1VAR(SC_RoomCreatedPacket, EPacketProtocol::SC_ROOM_CREATED, int32, roomId, room_id, true);
	/// <summary>
	/// Failed to join to a room packet for server
	/// </summary>
	/// <param name="errCause">Reason of couldn't join to the room</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_EMPTY_PACKET_1VAR(SC_RoomCreationFailedPacket, EPacketProtocol::SC_ROOM_CREATE_FAILED, RoomContract, errCause, cause, true);
	/// <summary>
	/// Joined to a room packet for server
	/// </summary>
	/// <param name="clientId">- An id of client</param>
	/// <param name="roomId">- An id of the room</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_EMPTY_PACKET_2VAR_WITH_DEFAULT(SC_RoomJoinedPacket, EPacketProtocol::SC_ROOM_JOINED, int32, clientId, user_id, -1, int32, roomId, room_id, -1);
	/// <summary>
	/// Notifying other joined to the current room packet for server
	/// </summary>
	/// <param name="clientId">- An id of the newie client</param>
	/// <param name="memberInfo">- Information of the newie</param>
	/// <remarks>Server would send it to the client</remarks>
	struct [[nodiscard]] SC_RoomOtherJoinedPacket : public FSagaBasicPacket
	{
		using Super = FSagaBasicPacket;

		[[nodiscard]]
		static consteval size_t WannabeSize() noexcept
		{
			return Super::MinSize() + sizeof(int32) + sizeof(datagrams::SerializedMember);
		}

		[[nodiscard]]
		static consteval ptrdiff_t SignedWannabeSize() noexcept
		{
			return static_cast<ptrdiff_t>(WannabeSize());
		}

		constexpr SC_RoomOtherJoinedPacket()
			noexcept(std::conjunction_v<std::is_nothrow_default_constructible<int32>, std::is_nothrow_default_constructible<datagrams::SerializedMember>>)
			: Super((EPacketProtocol::SC_ROOM_JOINED), static_cast<int16>(SignedWannabeSize()))
			, memberInfo(), roomId()
		{}

		constexpr SC_RoomOtherJoinedPacket(const int32& room_id, const datagrams::SerializedMember& info)
			noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<int32>, std::is_nothrow_copy_constructible<datagrams::SerializedMember>>)
			: Super((EPacketProtocol::SC_ROOM_JOINED), static_cast<int16>(SignedWannabeSize()))
			, memberInfo(info), roomId(room_id)
		{}

		constexpr SC_RoomOtherJoinedPacket(int32&& room_id, const datagrams::SerializedMember& info)
			noexcept(std::conjunction_v<std::is_nothrow_move_constructible<int32>, std::is_nothrow_copy_constructible<datagrams::SerializedMember>>)
			: Super((EPacketProtocol::SC_ROOM_JOINED), static_cast<int16>(SignedWannabeSize()))
			, memberInfo(info), roomId(std::move(room_id))
		{}

		constexpr SC_RoomOtherJoinedPacket(const int32& room_id, datagrams::SerializedMember&& info)
			noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<int32>, std::is_nothrow_move_constructible<datagrams::SerializedMember>>)
			: Super((EPacketProtocol::SC_ROOM_JOINED), static_cast<int16>(SignedWannabeSize()))
			, memberInfo(std::move(info)), roomId(room_id)
		{}

		constexpr SC_RoomOtherJoinedPacket(int32&& room_id, datagrams::SerializedMember&& info)
			noexcept(std::conjunction_v<std::is_nothrow_move_constructible<int32>, std::is_nothrow_move_constructible<datagrams::SerializedMember>>)
			: Super((EPacketProtocol::SC_ROOM_JOINED), static_cast<int16>(SignedWannabeSize()))
			, memberInfo(std::move(info)), roomId(std::move(room_id))
		{}

		MAKE_SERIALIZE_METHOD();
		MAKE_RW_METHODS();

		datagrams::SerializedMember memberInfo;
		int32 roomId;
	};
	/// <summary>
	/// Failed to join to a room packet for server
	/// </summary>
	/// <param name="errCause">- Reason of couldn't join to the room</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_EMPTY_PACKET_1VAR(SC_RoomJoinFailedPacket, EPacketProtocol::SC_ROOM_JOIN_FAILED, RoomContract, errCause, cause, true);
	/// <summary>
	/// Room left packet for server
	/// </summary>
	/// <param name="clientId">- An id of client</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_EMPTY_PACKET_1VAR_WITH_DEFAULT(SC_RoomLeftPacket, EPacketProtocol::SC_ROOM_LEFT, int32, clientId, user_id, -1);
	/// <summary>
	/// Creating a client packet for server
	/// </summary>
	/// <param name="clientId">- An id of client</param>
	/// <param name="roomId"/>
	/// <remarks>Server would send it to the client</remarks>
	struct SC_CreatePlayerPacket : public FSagaBasicPacket
	{
		using Super = FSagaBasicPacket;

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

		constexpr SC_CreatePlayerPacket(int32 id) noexcept
			: Super(EPacketProtocol::SC_CREATE_PLAYER, SignedWannabeSize())
			, clientId(id), userName()
		{}

		MAKE_SERIALIZE_METHOD();
		MAKE_RW_METHODS();

		int32 clientId;
		wchar_t userName[nickNameLength];
	};
	/// <summary>
	/// Remove a certain client packet for server
	/// </summary>
	/// <param name="clientId">- An id of client</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_EMPTY_PACKET_1VAR(SC_DestroyPlayerPacket, EPacketProtocol::SC_REMOVE_PLAYER, int32, clientId, user_id, true);
	/// <summary>
	/// Assigning ID to client packet for server
	/// </summary>
	/// <param name="clientId">- An id of client</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_EMPTY_PACKET_1VAR_WITH_DEFAULT(SC_SucceedSignInPacket, EPacketProtocol::SC_SIGNIN_SUCCESS, int32, clientId, user_id, -1);
	/// <summary>
	/// Assigning ID to client packet for server
	/// </summary>
	/// <param name="errCause">- Cause of the failure</param>
	/// <remarks>Server would send it to the client</remarks>
	MAKE_EMPTY_PACKET_1VAR(SC_FailedSignInPacket, EPacketProtocol::SC_SIGNIN_FAILURE, int, errCause, cause, true);
	/// <summary>
	/// Position packet for server
	/// </summary>
	/// <param name="clientId">- Target player's id</param>
	/// <param name="x"/>
	/// <param name="y"/>
	/// <param name="z"/>
	/// <remarks>Server would broadcast it to clients</remarks>
	struct SC_UpdatePositionPacket : public FSagaBasicPacket
	{
		using Super = FSagaBasicPacket;

		[[nodiscard]]
		static consteval size_t WannabeSize() noexcept
		{
			return Super::MinSize() + sizeof(int32) + sizeof(float) * 3;
		}

		[[nodiscard]]
		static consteval ptrdiff_t SignedWannabeSize() noexcept
		{
			return static_cast<ptrdiff_t>(Super::MinSize() + sizeof(int) + sizeof(float) * 3);
		}

		constexpr SC_UpdatePositionPacket()
			: SC_UpdatePositionPacket(-1, 0, 0, 0)
		{}

		constexpr SC_UpdatePositionPacket(int32 id, float px, float py, float pz) noexcept
			: Super(EPacketProtocol::SC_MOVE_CHARACTER, SignedWannabeSize())
			, clientId(id), x(px), y(py), z(pz)
		{}

		MAKE_SERIALIZE_METHOD();
		MAKE_RW_METHODS();

		int32 clientId;
		float x, y, z;
	};
}

#endif // !SAGAFRAMEWORK_NET_SC_PACKET_PREFABS_H
