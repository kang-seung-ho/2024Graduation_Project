#pragma once
#include "Saga/PlayerControllers/SagaInterfaceController.h"

#include "SagaSessionController.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Controller")
class SAGAFRAMEWORK_API ASagaSessionController : public ASagaInterfaceController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
