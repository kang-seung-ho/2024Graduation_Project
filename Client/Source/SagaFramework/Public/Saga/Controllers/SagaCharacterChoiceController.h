#pragma once
#include "Saga/Controllers/SagaInterfaceController.h"
#include <InputActionValue.h>

#include "SagaCharacterChoiceController.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Controller")
class SAGAFRAMEWORK_API ASagaCharacterChoiceController : public ASagaInterfaceController
{
	GENERATED_BODY()

public:
	//UPROPERTY()
	TSubclassOf<UUserWidget> mSelectWidgetClass;
	//UPROPERTY()
	class USagaCharacterSelectWidget* mSelectWidget;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* myHoveredCharacter;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* mySelectedCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isGameStartable;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isStartButtonClicked;

	UPROPERTY(BlueprintReadOnly)
	int32 Minutes = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 Seconds = 30;

	ASagaCharacterChoiceController(const FObjectInitializer& initializer);

	virtual void Tick(float delta_time) override;

	UFUNCTION()
	void CountDown();

	UFUNCTION()
	bool GetStartButtonStatus() const noexcept
	{
		return isStartButtonClicked;
	}

	UFUNCTION()
	AActor* GetSelectedActor() const noexcept
	{
		return mySelectedCharacter;
	}

	UFUNCTION()
	void OnClick(const FInputActionValue& input);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
};
