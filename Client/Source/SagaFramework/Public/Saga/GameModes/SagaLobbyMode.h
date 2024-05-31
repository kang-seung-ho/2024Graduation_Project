#pragma once
#include "Saga/GameModes/SagaInterfaceOnlyModeBase.h"

#include "SagaLobbyMode.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Game Mode")
class SAGAFRAMEWORK_API ASagaLobbyMode : public ASagaInterfaceOnlyModeBase
{
	GENERATED_BODY()

public:
	ASagaLobbyMode();
};
