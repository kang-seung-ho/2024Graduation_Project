export module Iconer.App.PacketProtocol;
import <cstdint>;
import <limits>;

export namespace iconer::app
{
	enum class [[nodiscard]] PacketProtocol : std::uint8_t
	{
		UNKNOWN = 0,

		CS_SIGNIN,
		CS_SIGNOUT,
		CS_SIGNUP,

		CS_REQUEST_VERSION,
		CS_REQUEST_ROOMS,
		CS_REQUEST_USERS,

		CS_ROOM_CREATE,
		CS_ROOM_DESTROY,
		CS_ROOM_JOIN,
		CS_ROOM_MATCH,
		CS_ROOM_LEAVE,

		CS_GAME_START, // 방장
		CS_GAME_EXIT,

		CS_MY_POSITION, // 좌표
		CS_MY_TRANSFORM, // 좌표, 회전각
		CS_MY_INPUT_PRESS,
		CS_MY_INPUT_RELEASE,
		CS_MY_ANIMATION_START,
		CS_MY_ANIMATION_END,

		CS_CHAT,
		CS_RPC, // 21
		CS_GAME_LOADED, // 22
		CS_READY,
		CS_SET_TEAM,

		CS_RPC_WALK,
		CS_RPC_RUN,
		CS_RPC_JUMP,
		CS_RPC_RIDE,
		CS_RPC_ATTACK_0, // Normal attack #1
		CS_RPC_ATTACK_1, // Normal attack #2
		CS_RPC_ATTACK_2, // Special Attack #3
		CS_RPC_ATTACK_3, // Special Attack #4
		CS_RPC_SKILL_0, // Ability #1
		CS_RPC_SKILL_1, // Ability #2
		CS_RPC_SKILL_2, // Ability #3
		CS_RPC_SKILL_3, // Ability #4
		CS_RPC_USE_ITEM_0,
		CS_RPC_USE_ITEM_1,
		CS_RPC_USE_ITEM_2,
		CS_RPC_USE_ITEM_3,
		CS_RPC_USE_ITEM_4,
		CS_RPC_CHANGE_HAND_ITEM,
		CS_RPC_DEAD,
		CS_UPDATE_ROOM = 50,

		SC_SIGNIN_SUCCESS = 120,
		SC_SIGNIN_FAILURE,
		SC_SIGNUP_SUCCESS,
		SC_SIGNUP_FAILURE,

		SC_SERVER_INFO,
		SC_RESPOND_ROOMS,
		SC_RESPOND_USERS,
		SC_RESPOND_VERSION,

		SC_ROOM_CREATED_BY_SELF,
		SC_ROOM_CREATED,
		SC_ROOM_CREATE_FAILED,
		SC_ROOM_DESTROYED,
		SC_ROOM_JOINED,
		SC_ROOM_JOIN_FAILED,
		SC_ROOM_LEFT,
		SC_ROOM_MEMBER_LEFT,

		SC_GAME_START,
		SC_GAME_VICTORY,
		SC_GAME_DEFEAT,

		SC_CREATE_PLAYER,
		SC_CREATE_OBJET,
		SC_CREATE_VFX,
		SC_MOVE_CHARACTER,
		SC_MOVE_OBJET,
		SC_LOOK_CHARACTER,
		SC_LOOK_OBJET,
		SC_UPDATE_CHARACTER,
		SC_UPDATE_OBJET,
		SC_REMOVE_CHARACTER,
		SC_REMOVE_OBJET,

		SC_PLAY_SFX,
		SC_PLAY_MUSIC,
		SC_ANIMATION_START,

		SC_CHAT,
		SC_RPC,
		SC_REMOVE_PLAYER,
		SC_GAME_GETTING_READY,
		SC_FAILED_GAME_START,
		SC_SET_TEAM,
	};
}
