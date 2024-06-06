#pragma once
#include "Saga/PlayerControllers/SagaInterfaceController.h"
#include <InputActionValue.h>

#include "SagaCharacterChoiceController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnClickedCharacter, class ASagaSelectCharacter*, character);

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Controller")
class SAGAFRAMEWORK_API ASagaCharacterChoiceController final : public ASagaInterfaceController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSagaSystem|Character Choice Level")
	FSagaEventOnClickedCharacter OnClickedCharacter;

	ASagaCharacterChoiceController(const FObjectInitializer& initializer) noexcept;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void OnClick(const FInputActionValue& Value);
};
