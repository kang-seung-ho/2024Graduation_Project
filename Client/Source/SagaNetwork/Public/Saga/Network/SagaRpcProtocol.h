#pragma once
#include "CoreMinimal.h"

#include "SagaRpcProtocol.generated.h"

#define ICONER_RPC_ENUM_ITEM(name) RPC_BEG_##name, RPC_END_##name,

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
	RPC_BEG_RIDE, // take on the guardian
	RPC_END_RIDE, // take off from the guardian
	RPC_SKILL_0 = 100, // Ability #1
	RPC_SKILL_1, // Ability #2
	RPC_SKILL_2, // Ability #3
	RPC_SKILL_3, // Ability #4
	RPC_USE_ITEM_0,
	RPC_USE_ITEM_1,
	RPC_USE_ITEM_2,
	RPC_USE_ITEM_3,
	RPC_USE_ITEM_4,
	RPC_SPAWN_ITEM,
	RPC_GRAB_ITEM,
	RPC_CHANGE_HAND_ITEM,
	RPC_DMG_PLYER,
	RPC_DMG_GUARDIAN,
	RPC_DMG_GUARDIANS_PART,
	RPC_DEAD,
};
