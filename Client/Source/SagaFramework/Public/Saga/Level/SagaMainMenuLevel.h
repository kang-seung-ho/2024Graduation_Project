#pragma once
#include "Saga/Level/SagaLevelScripter.h"

#include "SagaMainMenuLevel.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|Level")
class SAGAFRAMEWORK_API ASagaMainMenuLevel final : public ASagaLevelScripter
{
	GENERATED_BODY()

public:
	ASagaMainMenuLevel() noexcept;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnFailedToConnect(ESagaConnectionContract reason);
	UFUNCTION()
	void OnSignedIn(int32 my_id, const FName& nickname);

private:
	UPROPERTY()
	TSubclassOf<class USagaUserWidget> levelUiClass;
	UPROPERTY()
	TObjectPtr<class USagaMainMenuUiWidget> levelUiInstance;
	UPROPERTY()
	TSubclassOf<class USagaUiPopup> errorPopupWidgetClass;
	UPROPERTY()
	TObjectPtr<class USagaUiPopup> errorPopupWidgetInstance;
};
