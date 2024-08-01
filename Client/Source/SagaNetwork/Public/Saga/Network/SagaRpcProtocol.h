#pragma once
#include <CoreMinimal.h>

#include "SagaRpcProtocol.generated.h"

#define ICONER_RPC_ENUM_ITEM(name) RPC_BEG_##name, RPC_END_##name

UENUM(BlueprintType)
enum class [[nodiscard]] ESagaRpcProtocol : uint8
{
	RPC_UNKNOWN = 0,
	RPC_BEG_WALK, RPC_END_WALK,
	RPC_BEG_RUN, RPC_END_RUN,
	RPC_BEG_JUMP, RPC_END_JUMP,
	RPC_BEG_ATTACK_0, RPC_END_ATTACK_0, // Normal attack #1
	RPC_BEG_ATTACK_1, RPC_END_ATTACK_1, // Normal attack #2
	RPC_BEG_ATTACK_2, RPC_END_ATTACK_2, // Normal attack #3
	RPC_BEG_ATTACK_3, RPC_END_ATTACK_3, // Normal attack #4
	RPC_BEG_RIDE, RPC_END_RIDE,

	RPC_POSITION = 20,
	RPC_ROTATION,

	RPC_SKILL_0 = 50, // Ability #1
	RPC_SKILL_1, // Ability #2
	RPC_SKILL_2, // Ability #3
	RPC_SKILL_3, // Ability #4

	RPC_USE_ITEM_0 = 100,
	RPC_USE_ITEM_1,
	RPC_USE_ITEM_2,
	RPC_USE_ITEM_3,
	RPC_USE_ITEM_4,
	RPC_ASSIGN_ITEM_ID,
	RPC_DESTROY_ITEM_BOX,
	RPC_SPAWN_ITEM,
	RPC_GRAB_ITEM,

	RPC_MAIN_WEAPON = 140,
	RPC_CHANGE_HAND_ITEM,
	/// <summary>
	/// 클라에서 준비를 하면 서버에서 준비된 플레이어의 수를 보냄
	/// <para>-------------------------------------------------------------------------------</para>
	/// </summary>
	/// <param name="arg0">- 준비된 플레이어의 수</param>
	/// <param name="arg1">- 없음</param>
	RPC_NOTIFY_READY_COUNTER,

	RPC_DMG_PLYER = 180,
	RPC_DMG_GUARDIAN,
	RPC_DMG_GUARDIANS_PART,
	RPC_DMG_BASE,
	RPC_DEAD,
	RPC_RESPAWN,
	RPC_RESPAWN_TIMER,

	RPC_DESTROY_CORE = 220,
	RPC_GET_SCORE,
	RPC_CHECK_GAME_VICTORY,
	RPC_UPDATE_HEALTH,
	RPC_WEAPON_TIMER, // seconds
	RPC_GAME_TIMER, // seconds
	/// <summary>
	/// 클라에서 카운트 다운을 요청하면, 서버에서 남은 시간을 보냄
	/// <para>-------------------------------------------------------------------------------</para>
	/// </summary>
	/// <param name="arg0">- 준비된 플레이어의 수</param>
	/// <param name="arg1">- 없음</param>
	RPC_NOTIFY_GAME_COUNTDOWN,
};
