#pragma once
#include "Saga/GameModes/SagaInterfaceOnlyModeBase.h"

#include "SagaMainMenuMode.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Game Mode")
class SAGAFRAMEWORK_API ASagaMainMenuMode : public ASagaInterfaceOnlyModeBase
{
	GENERATED_BODY()

public:
	ASagaMainMenuMode();
};
