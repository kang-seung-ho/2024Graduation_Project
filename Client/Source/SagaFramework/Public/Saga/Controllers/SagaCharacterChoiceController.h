#pragma once
#include "Saga/Controllers/SagaInterfaceController.h"

#include "SagaCharacterChoiceController.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Controller")
class SAGAFRAMEWORK_API ASagaCharacterChoiceController : public ASagaInterfaceController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
