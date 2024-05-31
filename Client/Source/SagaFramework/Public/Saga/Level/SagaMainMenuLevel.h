#pragma once
#include "Saga/Level/SagaLevelScripter.h"

#include "SagaMainMenuLevel.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|Level", meta = (DisplayName = "Saga Main Menu Level C++ Class"))
class SAGAFRAMEWORK_API ASagaMainMenuLevel final : public ASagaLevelScripter
{
	GENERATED_BODY()

public:
	ASagaMainMenuLevel() noexcept;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

private:
	UFUNCTION()
	void HandleStartPressed();

	UFUNCTION()
	void OnFailedToConnect(ESagaConnectionContract reason);
	UFUNCTION()
	void OnSignedIn(int32 my_id, const FText& nickname);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USagaUserWidget> levelUiClass;
	UPROPERTY()
	TObjectPtr<class USagaMainMenuUiWidget> levelUiInstance;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USagaUiPopup> errorPopupWidgetClass;
	UPROPERTY()
	TObjectPtr<class USagaUiPopup> errorPopupWidgetInstance;
};
