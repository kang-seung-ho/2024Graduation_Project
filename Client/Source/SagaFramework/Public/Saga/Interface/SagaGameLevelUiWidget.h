#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"

#include "SagaGameLevelUiWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Game Level UI Widget C++ Class"))
class SAGAFRAMEWORK_API USagaGameLevelUiWidget final : public USagaLiveUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI|Game")
	TObjectPtr<class UTextBlock> myMinutesLabel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI|Game")
	TObjectPtr<class UTextBlock> mySecondsLabel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI|Game")
	int32 myMinutes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI|Game")
	int32 mySeconds;

	USagaGameLevelUiWidget(const FObjectInitializer& initializer) noexcept;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "CandyLandSaga|UI|Game")
	void SetMinutes(const int32 minutes) noexcept;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "CandyLandSaga|UI|Game")
	void SetSeconds(const int32 seconds) noexcept;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
};
