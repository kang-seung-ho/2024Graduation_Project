#pragma once
#include <CoreMinimal.h>
#include <GameFramework/PlayerController.h>

#include "SagaGameEndController.generated.h"

UCLASS()
class SAGAGAME_API ASagaGameEndController : public APlayerController
{
	GENERATED_BODY()

public:
	ASagaGameEndController();
	virtual void BeginPlay() override;

public:
	UPROPERTY()
	TSubclassOf<UUserWidget> mGameResultClass;
	UPROPERTY()
	class UUserWidget* mGameResultWidget;
	
};
