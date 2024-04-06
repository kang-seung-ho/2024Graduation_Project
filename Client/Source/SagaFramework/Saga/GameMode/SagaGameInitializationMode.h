#pragma once
#include "Saga/GameMode/SagaUserInterfaceModeBase.h"

#include "SagaGameInitializationMode.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game Mode")
class SAGAFRAMEWORK_API ASagaGameInitializationMode : public ASagaUserInterfaceModeBase
{
	GENERATED_BODY()

public:
	ASagaGameInitializationMode();
};
