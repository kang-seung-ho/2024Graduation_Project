#pragma once
#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>

#include "SagaInGameMode.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Game Mode")
class SAGAGAME_API ASagaInGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ASagaInGameMode();
};
