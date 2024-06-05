#pragma once
#include "SagaGame.h"
#include <GameFramework/PlayerController.h>

#include "SagaGameEndController.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Game Mode")
class SAGAGAME_API ASagaGameEndController : public APlayerController
{
	GENERATED_BODY()

public:
	ASagaGameEndController() noexcept;

	virtual void BeginPlay() override;

public:
	UPROPERTY()
	TSubclassOf<UUserWidget> mGameResultClass;
	UPROPERTY()
	TObjectPtr<class UUserWidget> mGameResultWidget;
};
