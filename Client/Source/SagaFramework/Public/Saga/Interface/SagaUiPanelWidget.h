#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"

#include "SagaUiPanelWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|UI")
class SAGAFRAMEWORK_API USagaUiPanelWidget : public USagaLiveUserWidget
{
	GENERATED_BODY()

public:
	USagaUiPanelWidget(const FObjectInitializer& initializer) noexcept;
};
