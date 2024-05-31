#pragma once
#include "Saga/Controllers/SagaInterfaceController.h"

#include "SagaLobbyController.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Controller")
class SAGAFRAMEWORK_API ASagaLobbyController : public ASagaInterfaceController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
