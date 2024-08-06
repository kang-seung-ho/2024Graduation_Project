#pragma once
#include "SagaFramework.h"
#include <GameFramework/PlayerController.h>

#include "SagaInterfaceController.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|System|Controller")
class SAGAFRAMEWORK_API ASagaInterfaceController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
