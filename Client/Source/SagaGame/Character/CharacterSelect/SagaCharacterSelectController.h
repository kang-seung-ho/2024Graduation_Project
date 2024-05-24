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

UCLASS(BlueprintType, Category = "CandyLandSagaSystem|System|Controller")
class SAGAGAME_API ASagaCharacterSelectController final : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSagaSystem|System|Character Choice Level")
	bool isGameStartable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSagaSystem|System|Character Choice Level")
	int32 Minutes = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSagaSystem|System|Character Choice Level")
	int32 Seconds = 30;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSagaSystem|System|Character Choice Level")
	FTimespan awaitCountdownDuration;

	ASagaCharacterSelectController(const FObjectInitializer& initializer) noexcept;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void CountDown();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void OnClick(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USagaCharacterSelectWidget> levelUiClass;
	UPROPERTY()
	TObjectPtr<class USagaCharacterSelectWidget> levelUiInstance;

	UPROPERTY()
	TObjectPtr<AActor> mUnderCursorActor;
	UPROPERTY()
	TObjectPtr<AActor> mSelectActor;
};
