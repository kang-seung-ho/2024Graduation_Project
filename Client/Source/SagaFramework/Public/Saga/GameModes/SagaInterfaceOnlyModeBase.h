#pragma once
#include "Saga/GameModes/SagaGameModeBase.h"

#include "SagaInterfaceOnlyModeBase.generated.h"

UCLASS(Blueprintable, BlueprintType, Category = "CandyLandSaga|System|Game Mode")
class SAGAFRAMEWORK_API ASagaInterfaceOnlyModeBase : public ASagaGameModeBase
{
	GENERATED_BODY()

public:
	ASagaInterfaceOnlyModeBase(const FObjectInitializer& initializer) noexcept;
};
