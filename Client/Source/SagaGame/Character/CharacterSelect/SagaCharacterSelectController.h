#pragma once
#include "SagaGame.h"
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <UObject/ObjectPtr.h>
#include <GameFramework/Actor.h>
#include <GameFramework/PlayerController.h>
#include <InputActionValue.h>
#include <Templates/SubclassOf.h>
#include <Blueprint/UserWidget.h>

#include "SagaCharacterSelectController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnClickedCharacter, class ASagaSelectCharacter*, character);

UCLASS(BlueprintType, Category = "CandyLandSagaSystem|System|Controller")
class SAGAGAME_API ASagaCharacterSelectController final : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSagaSystem|System|Character Choice Level")
	TObjectPtr<AActor> mSelectActor;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSagaSystem|System|Character Choice Level")
	FSagaEventOnClickedCharacter OnClickedCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSagaSystem|System|Character Choice Level")
	int32 Minutes = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSagaSystem|System|Character Choice Level")
	int32 Seconds = 30;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSagaSystem|System|Character Choice Level")
	FTimespan awaitCountdownDuration;

	ASagaCharacterSelectController(const FObjectInitializer& initializer) noexcept;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void OnClick(const FInputActionValue& Value);
};
