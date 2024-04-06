#pragma once
#include "Saga/GameMode/SagaUserInterfaceModeBase.h"

#include "SagaNetworkReadyMode.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game Mode")
class SAGAFRAMEWORK_API ASagaNetworkReadyMode : public ASagaUserInterfaceModeBase
{
	GENERATED_BODY()

public:
	ASagaNetworkReadyMode();
};
