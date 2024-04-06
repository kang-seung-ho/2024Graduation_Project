#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "SagaPlayerControllerBase.generated.h"

UCLASS(BlueprintType, Abstract, Category = "CandyLandSaga|Game Mode|Controller")
class SAGAFRAMEWORK_API ASagaPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
};
