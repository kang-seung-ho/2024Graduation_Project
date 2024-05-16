#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"
#include <Components/EditableTextBox.h>

#include "SagaMainMenuUiWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Category = "CandyLandSaga|UI")
class SAGAFRAMEWORK_API USagaMainMenuUiWidget final : public USagaLiveUserWidget
{
	GENERATED_BODY()

public:
	USagaMainMenuUiWidget(const FObjectInitializer& initializer) noexcept;


	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void OnSignInPressed() noexcept;

private:
	UPROPERTY()
	TObjectPtr<UEditableTextBox> inputBox;
};
