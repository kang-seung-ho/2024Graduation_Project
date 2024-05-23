#pragma once
#include "SagaGame.h"
#include "SagaGameInfo.h"

#include "Saga/Interface/SagaUserWidget.h"
#include "SagaCharacterSelectWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Character Choice Level UI Widget C++ Class"))
class SAGAGAME_API USagaCharacterSelectWidget : public USagaUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<class USagaTimerUiWidget> myTimer;
	UPROPERTY()
	TObjectPtr<class USagaUiButton> myStartButton;

	USagaCharacterSelectWidget(const FObjectInitializer& initializer) noexcept;

	UFUNCTION()
	void StartButtonEnable(const bool flag);

protected:
	virtual void NativePreConstruct();
	virtual void NativeTick(const FGeometry& geometry, float delta_time);
};
