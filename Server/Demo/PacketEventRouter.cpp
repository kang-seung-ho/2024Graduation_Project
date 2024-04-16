module;
#include <atomic>

#define ICONER_UNLIKELY [[unlikely]]
#define ICONER_LIKELY [[likely]]

module Demo.PacketProcessor;
import Iconer.Utility.Serializer;
import Iconer.Application.BasicPacket;
import Iconer.Application.Resources.String;

using namespace iconer::app;

ptrdiff_t
demo::PacketProcessor(demo::Framework& framework
	, User& user
	, std::span<std::byte> packet_data
	, ptrdiff_t last_offset)
{
	if (nullptr == packet_data.data()) ICONER_UNLIKELY
	{
		constexpr auto & msg = GetResourceString<3>();
		throw msg.data();
	};

	PacketProtocol protocol;
	std::int16_t pk_size = 0;
	const std::byte* last_buf = iconer::util::Deserialize(iconer::util::Deserialize(packet_data.data(), protocol), pk_size);

	if (pk_size <= 0) ICONER_UNLIKELY
	{
		constexpr auto & msg = GetResourceString<4>();
		throw msg.data();
	};

	constexpr auto& unknown_packet_errmsg = GetResourceString<5>();
	constexpr auto& notsupported_packet_errmsg = GetResourceString<11>();

	if (pk_size <= last_offset)
	{
		switch (protocol)
		{
		case PacketProtocol::UNKNOWN:
		{
			throw unknown_packet_errmsg.data();
		}

		case PacketProtocol::CS_SIGNIN:
		{
			throw notsupported_packet_errmsg.data();
		}
		break;

		case PacketProtocol::CS_SIGNOUT:
		{
			OnSignOut(user);
		}
		break;

		case PacketProtocol::CS_SIGNUP:
		{

		}
		break;

		case PacketProtocol::CS_REQUEST_VERSION:
		{
			// Empty packet
			OnRequestVersion(framework, user);
		}
		break;

		case PacketProtocol::CS_REQUEST_ROOMS:
		{
			// Empty packet
			OnRequestRoomList(framework, user);
		}
		break;

		case PacketProtocol::CS_REQUEST_USERS:
		{
			// Empty packet
			OnRequestMemberList(framework, user);
		}
		break;

		case PacketProtocol::CS_ROOM_CREATE:
		{
			wchar_t room_title[16]{};
			packets::Deserialize(last_buf, room_title);

			OnCreateRoom(framework, user, room_title);
		}
		break;

		case PacketProtocol::CS_ROOM_DESTROY:
		{
			// Empty packet

		}
		break;

		case PacketProtocol::CS_ROOM_JOIN:
		{
			std::int32_t room_id{};
			packets::Deserialize(last_buf, room_id);

			OnJoinRoom(framework, user, room_id);
		}
		break;

		case PacketProtocol::CS_ROOM_MATCH:
		{
			// Empty packet
		}
		break;

		case PacketProtocol::CS_ROOM_LEAVE:
		{
			// Empty packet
			OnLeaveRoom(framework, user);
		}
		break;

		case PacketProtocol::CS_GAME_START:
		{
			// Empty packet
			OnGameStartSignal(framework, user);
		}
		break;

		case PacketProtocol::CS_GAME_LOADED:
		{
			// Empty packet
			OnGameLoadedSignal(framework, user);
		}
		break;

		case PacketProtocol::CS_GAME_EXIT:
		{
		}
		break;

		case PacketProtocol::CS_MY_POSITION:
		{
			float px{}, py{}, pz{};
			packets::Deserialize(last_buf, px, py, pz);

			OnReceivePosition(framework, user, px, py, pz);
		}
		break;

		case PacketProtocol::CS_MY_TRANSFORM:
		{
			float pr{}; // roll (x)
			float py{}; // yaw (y)
			float pp{}; // pitch (z)
			packets::Deserialize(last_buf, pr, py, pp);

			OnReceiveRotation(framework, user, pr, pr, pp);
		}
		break;

		case PacketProtocol::CS_MY_INPUT_PRESS:
		{
		}
		break;

		case PacketProtocol::CS_MY_INPUT_RELEASE:
		{
		}
		break;

		case PacketProtocol::CS_MY_ANIMATION_START:
		{
		}
		break;

		case PacketProtocol::CS_MY_ANIMATION_END:
		{
		}
		break;

		case PacketProtocol::CS_CHAT:
		{
		}
		break;

		case PacketProtocol::CS_SET_TEAM:
		{
			std::int8_t team_id{};

			packets::Deserialize(last_buf, team_id);

			// 0 :  red team
			// 1 : blue team
			OnTeamChanged(framework, user, team_id == 0);
		}
		break;

		case PacketProtocol::CS_RPC:
		{
			RpcProtocol category{};
			std::int64_t arg0{};
			std::int32_t arg1{};

			packets::Deserialize(last_buf, category, arg0, arg1);

			OnRpc(framework, user, category, arg0, arg1);
		}
		break;

		default:
		{
			throw unknown_packet_errmsg.data();
		}
		}

		return pk_size;
	}
	else
	{
		return 0;
	}
}
