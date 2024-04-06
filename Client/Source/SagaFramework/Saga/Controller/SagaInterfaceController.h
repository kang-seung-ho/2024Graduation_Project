#pragma once
#include "Saga/Controller/SagaPlayerControllerBase.h"

#include "SagaInterfaceController.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game Mode|Controller")
class SAGAFRAMEWORK_API ASagaInterfaceController : public ASagaPlayerControllerBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupInputComponent() override;
};
