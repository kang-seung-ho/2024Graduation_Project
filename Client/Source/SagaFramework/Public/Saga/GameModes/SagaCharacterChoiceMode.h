#pragma once
#include "Saga/GameModes/SagaInterfaceOnlyModeBase.h"

#include "SagaCharacterChoiceMode.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSagaSystem||Game Mode")
class SAGAFRAMEWORK_API ASagaCharacterChoiceMode : public ASagaInterfaceOnlyModeBase
{
	GENERATED_BODY()

public:
	ASagaCharacterChoiceMode();
};
