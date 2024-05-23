#pragma once
#include "Saga/Interface/SagaUserWidget.h"

#include "SagaTimerUiWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Timer UI Widget C++ Class"))
class SAGAFRAMEWORK_API USagaTimerUiWidget : public USagaUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI|Timer UI Widget")
	TObjectPtr<class UTextBlock> myMinutesLabel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI|Timer UI Widget")
	TObjectPtr<class UTextBlock> mySecondsLabel;

	USagaTimerUiWidget(const FObjectInitializer& initializer) noexcept;

	UFUNCTION()
	void SetMinutes(const int32 minutes) noexcept;
	UFUNCTION()
	void SetSeconds(const int32 seconds) noexcept;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
};
