#pragma once
#ifndef SAGAFRAMEWORK_NET_CS_PACKET_PREFABS_H
#define SAGAFRAMEWORK_NET_CS_PACKET_PREFABS_H

#include "SagaNetwork.h"
#include <utility>
#include <string>
#include <string_view>

#include "Saga/Network/SagaBasicPacket.h"
#include "Saga/Network/SagaPacketProtocol.h"
#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaPacketHelper.inl"

namespace saga::inline cs
{
#pragma pack(push, 1)
	/// <summary>
	/// RPC packet for client
	/// </summary>
	/// <param name="rpcScript">- The category of rpc msg</param>
	/// <param name="arg0">- 64bit rpc argument</param>
	/// <param name="arg1">- 32bit rpc argument</param>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET_3VAR(CS_DeterRpcPacket, EPacketProtocol::CS_RPC, ESagaRpcProtocol, rpcProtocol, int64, arg0, int32, arg1);
	/// <summary>
	/// Team setter packet for client
	/// </summary>
	/// <param name="teamId">Team's id of user</param>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_PACKET_1VAR_WITH_DEFAULT(CS_SetTeamPacket, EPacketProtocol::CS_SET_TEAM, int8, teamId, team_id, 0);
	/// <summary>
	/// Requesting game version packet for client
	/// </summary>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET(CS_RequestVersionPacket, EPacketProtocol::CS_REQUEST_VERSION);
	/// <summary>
	/// Requesting every room packet for client
	/// </summary>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET(CS_RequestRoomsPacket, EPacketProtocol::CS_REQUEST_ROOMS);
	/// <summary>
	/// Requesting users in the room packet for client
	/// </summary>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET(CS_RequestMembersPacket, EPacketProtocol::CS_REQUEST_USERS);
	/// <summary>
	/// Game initiating packet for client
	/// </summary>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET(CS_GameStartPacket, EPacketProtocol::CS_GAME_START);
	/// <summary>
	/// Notifying game's loading is done packet for client
	/// </summary>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET(CS_GameLoadedPacket, EPacketProtocol::CS_GAME_LOADED);
	/// <summary>
	/// Room create packet for client
	/// </summary>
	/// <param name="roomTitle">Title of the new room</param>
	/// <remarks>Client would send it to the server</remarks>
	struct SAGANETWORK_API CS_CreateRoomPacket : public FSagaBasicPacket
	{
		using Super = FSagaBasicPacket;

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
			: Super(EPacketProtocol::CS_ROOM_CREATE, SignedWannabeSize())
			, roomTitle()
		{
		}

		explicit constexpr CS_CreateRoomPacket(const wchar_t* begin, const wchar_t* end)
			: Super(EPacketProtocol::CS_ROOM_CREATE, SignedWannabeSize())
			, roomTitle()
		{
			std::copy(begin, end, roomTitle);
		}

		explicit constexpr CS_CreateRoomPacket(const wchar_t* nts, const size_t length)
			: Super(EPacketProtocol::CS_ROOM_CREATE, SignedWannabeSize())
			, roomTitle()
		{
			std::copy_n(nts, std::min(length, roomTitleLength), roomTitle);
		}

		template<size_t Length>
		explicit constexpr CS_CreateRoomPacket(const wchar_t(&str)[Length])
			: Super(EPacketProtocol::CS_ROOM_CREATE, SignedWannabeSize())
			, roomTitle()
		{
			std::copy_n(str, std::min(Length, roomTitleLength), roomTitle);
		}

		template<size_t Length>
		explicit constexpr CS_CreateRoomPacket(wchar_t(&& str)[Length])
			: Super(EPacketProtocol::CS_ROOM_CREATE, SignedWannabeSize())
			, roomTitle()
		{
			std::move(str, str + std::min(Length, roomTitleLength), roomTitle);
		}

		MAKE_WRITE_METHODS();
		MAKE_READ_METHOD();

		wchar_t roomTitle[roomTitleLength];
	};
	/// <summary>
	/// Room entering packet for client
	/// </summary>
	/// <param name="roomId"/>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_PACKET_1VAR_WITH_DEFAULT(CS_EnterRoomPacket, EPacketProtocol::CS_ROOM_JOIN, int32, roomId, room_id, -1);
	/// <summary>
	/// Room leaving packet for client
	/// </summary>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_EMPTY_PACKET(CS_LeaveRoomPacket, EPacketProtocol::CS_ROOM_LEAVE);
	/// <summary>
	/// Position packet for client
	/// </summary>
	/// <param name="x"/>
	/// <param name="y"/>
	/// <param name="z"/>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_PACKET_3VAR(CS_UpdatePositionPacket, EPacketProtocol::CS_MY_POSITION, float, x, px, float, y, py, float, z, pz);
	/// <summary>
	/// Rotation packet for client
	/// </summary>
	/// <param name="r">roll</param>
	/// <param name="y">yaw</param>
	/// <param name="p">pitch</param>
	/// <remarks>Client would send it to the server</remarks>
	MAKE_PACKET_3VAR(CS_UpdateRotationPacket, EPacketProtocol::CS_MY_TRANSFORM, float, r, pr, float, y, py, float, p, pp);
	/// <summary>
	/// Login packet for client
	/// </summary>
	/// <param name="userName">Nickname of client</param>
	/// <remarks>Client would send it to the server</remarks>
	struct SAGANETWORK_API CS_SignInPacket : public FSagaBasicPacket
	{
		using Super = FSagaBasicPacket;

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
			: Super(EPacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, userName()
		{
		}

		explicit constexpr CS_SignInPacket(const wchar_t* begin, const wchar_t* end)
			: Super(EPacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, userName()
		{
			std::copy(begin, end, userName);
		}

		explicit constexpr CS_SignInPacket(const wchar_t* nts, const size_t length)
			: Super(EPacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, userName()
		{
			std::copy_n(nts, std::min(length, nickNameLength), userName);
		}

		template<size_t Length>
		explicit constexpr CS_SignInPacket(const wchar_t(&str)[Length])
			: Super(EPacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, userName()
		{
			std::copy_n(str, std::min(Length, nickNameLength), userName);
		}

		template<size_t Length>
		explicit constexpr CS_SignInPacket(wchar_t(&& str)[Length])
			: Super(EPacketProtocol::CS_SIGNIN, SignedWannabeSize())
			, userName()
		{
			std::move(str, str + std::min(Length, nickNameLength), userName);
		}

		MAKE_WRITE_METHODS();
		MAKE_READ_METHOD();

		wchar_t userName[nickNameLength];
	};
	/// <summary>
	/// RPC packet for client
	/// </summary>
	/// <param name="rpcScript">- A descriptor for rpc msg</param>
	/// <param name="rpcArgument">- Single rpc argument</param>
	/// <remarks>Client would send it to the server</remarks>
	struct SAGANETWORK_API CS_RpcPacket : public FSagaBasicPacket
	{
		using Super = FSagaBasicPacket;

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

		explicit constexpr CS_RpcPacket() noexcept
			: Super(EPacketProtocol::CS_RPC, SignedWannabeSize())
			, rpcScript(), rpcArgument()
		{}

		explicit constexpr CS_RpcPacket(long long arg) noexcept
			: Super(EPacketProtocol::CS_RPC, SignedWannabeSize())
			, rpcScript(), rpcArgument(arg)
		{}

		explicit constexpr CS_RpcPacket(long long arg, const char* begin, const char* end) noexcept
			: CS_RpcPacket(arg)
		{
			std::copy(begin, end, rpcScript);
		}

		explicit constexpr CS_RpcPacket(long long arg, const char* nts, const size_t length) noexcept
			: CS_RpcPacket(arg)
		{
			std::copy_n(nts, std::min(length, msgLength), rpcScript);
		}

		explicit constexpr CS_RpcPacket(const char* begin, const char* end) noexcept
			: CS_RpcPacket(0, begin, end)
		{}

		explicit constexpr CS_RpcPacket(const char* nts, const size_t length) noexcept
			: CS_RpcPacket(0, nts, length)
		{}

		template<size_t Length>
		explicit constexpr CS_RpcPacket(const char(&str)[Length]) noexcept
			: CS_RpcPacket()
		{
			std::copy_n(str, std::min(Length, msgLength), rpcScript);
		}

		template<size_t Length>
		explicit constexpr CS_RpcPacket(char(&& str)[Length]) noexcept
			: CS_RpcPacket()
		{
			std::move(str, str + std::min(Length, msgLength), rpcScript);
		}

		template<size_t Length>
		explicit constexpr CS_RpcPacket(long long arg, const char(&str)[Length]) noexcept
			: CS_RpcPacket(arg)
		{
			std::copy_n(str, std::min(Length, msgLength), rpcScript);
		}

		template<size_t Length>
		explicit constexpr CS_RpcPacket(long long arg, char(&& str)[Length]) noexcept
			: CS_RpcPacket(arg)
		{
			std::move(str, str + std::min(Length, msgLength), rpcScript);
		}

		MAKE_WRITE_METHODS();
		MAKE_READ_METHOD();

		char rpcScript[msgLength];
		long long rpcArgument;
	};
#pragma pack(pop)
}

#endif // !SAGAFRAMEWORK_NET_CS_PACKET_PREFABS_H
