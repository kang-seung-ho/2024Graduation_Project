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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI|Timer UI Widget")
	int32 myMinutes;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI|Timer UI Widget")
	int32 mySeconds;

	USagaTimerUiWidget(const FObjectInitializer& initializer) noexcept;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "CandyLandSaga|UI|Timer UI Widget")
	void SetMinutes(const int32 minutes) noexcept;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "CandyLandSaga|UI|Timer UI Widget")
	void SetSeconds(const int32 seconds) noexcept;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
};
