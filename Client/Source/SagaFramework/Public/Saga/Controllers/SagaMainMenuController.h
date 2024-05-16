#pragma once
#include "Saga/Controllers/SagaInterfaceController.h"
#include "Saga/Network/SagaNetworkView.h"

#include "SagaMainMenuController.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Controller")
class SAGAFRAMEWORK_API ASagaMainMenuController : public ASagaInterfaceController, public ISagaNetworkView
{
	GENERATED_BODY()

public:
	ASagaMainMenuController();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Level", meta = (WorldContext = "world"))
	static class USagaMainMenuUiWidget* SpawnUI(UWorld* world, UClass* const uclass);

	UPROPERTY()
	TSubclassOf<class USagaUserWidget> levelUiClass;
	UPROPERTY()
	TObjectPtr<class USagaMainMenuUiWidget> levelUiInstance;
};
