#pragma once
#include "Saga/Controllers/SagaInterfaceController.h"

#include "SagaMainMenuController.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Controller")
class SAGAFRAMEWORK_API ASagaMainMenuController : public ASagaInterfaceController
{
	GENERATED_BODY()

public:
	ASagaMainMenuController();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<class USagaUserWidget> levelUiClass;
	UPROPERTY()
	TObjectPtr<class USagaMainMenuUiWidget> levelUiInstance;
};
