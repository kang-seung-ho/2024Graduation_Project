#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"

#include "SagaUiPanelWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|UI")
class SAGAFRAMEWORK_API USagaUiPanelWidget : public USagaLiveUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|UI", meta = (ExposeOnSpawn = "true"))
	bool showShroud;

public:
	USagaUiPanelWidget(const FObjectInitializer& initializer) noexcept;

};
