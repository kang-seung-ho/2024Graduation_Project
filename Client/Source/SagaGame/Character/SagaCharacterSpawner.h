#pragma once
#include <GameFramework/PlayerStart.h>

#include "SagaCharacterSpawner.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game")
class SAGAGAME_API ASagaCharacterSpawner : public APlayerStart
{
	GENERATED_BODY()
	
public:	
	ASagaCharacterSpawner(const FObjectInitializer& initializer) noexcept;
};
