#pragma once
#include "Saga/GameModes/SagaInterfaceOnlyModeBase.h"

#include "SagaSessionMode.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Game Mode")
class SAGAFRAMEWORK_API ASagaSessionMode : public ASagaInterfaceOnlyModeBase
{
	GENERATED_BODY()

public:
	ASagaSessionMode();
};
