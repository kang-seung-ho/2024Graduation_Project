#pragma once

#include "EngineMinimal.h"
#include "EngineGlobals.h"
#include "Engine.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"

#include "SagaAIInfo.generated.h"

UENUM(BlueprintType)
enum class EAIMonsterAnim : uint8
{
	Idle,
	Walk,
	Run,
	Attack,
	Death
};
