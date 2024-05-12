#pragma once
#include "../../SagaGameInfo.h"
#include "GameFramework/PlayerController.h"

#include "SagaCharacterSelectController.generated.h"

UCLASS()
class SAGAGAME_API ASagaCharacterSelectController : public APlayerController
{
	GENERATED_BODY()

public:
	ASagaCharacterSelectController();

public:
	bool isStartButtonClicked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isGameStartable;

	bool getStartButtonStatus()
	{
		return isStartButtonClicked;
	}

protected:
	AActor* mUnderCursorActor;
	AActor* mSelectActor;

	TSubclassOf<UUserWidget> mSelectWidgetClass;
	class USagaCharacterSelectWidget* mSelectWidget;

public:
	AActor* GetSelectActor() const
	{
		return mSelectActor;
	}

public:
	void CountDown();

	UPROPERTY(BlueprintReadOnly)
	int32 Minutes = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 Seconds = 30;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void SetupInputComponent();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	void OnClick(const FInputActionValue& Value);

};
