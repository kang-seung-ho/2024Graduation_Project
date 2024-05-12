#pragma once
#include "../SagaGameInfo.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"

#include "Saga/Interface/SagaUserWidget.h"
#include "SagaCharacterSelectWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|UI")
class SAGAGAME_API USagaCharacterSelectWidget : public USagaUserWidget
{
	GENERATED_BODY()

protected:
	UButton* mStartButton;

protected:
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	UFUNCTION()
	void StartButtonEnable(bool bEnable);
};
