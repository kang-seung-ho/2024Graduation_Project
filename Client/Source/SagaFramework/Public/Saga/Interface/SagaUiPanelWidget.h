#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"

#include "SagaUiPanelWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Panel Widget C++ Class"))
class SAGAFRAMEWORK_API USagaUiPanelWidget : public USagaLiveUserWidget
{
	GENERATED_BODY()

public:
	USagaUiPanelWidget(const FObjectInitializer& initializer) noexcept;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& geometry, float delta_time) override;

	UFUNCTION()
	FVector2D CalculateTargetScale() const noexcept;
};
