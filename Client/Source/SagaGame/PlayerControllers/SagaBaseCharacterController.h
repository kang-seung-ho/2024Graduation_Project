#pragma once
#include "SagaGame.h"
#include <GameFramework/PlayerController.h>

#include "SagaBaseCharacterController.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|System")
class SAGAGAME_API ASagaBaseCharacterController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASagaBaseCharacterController(const FObjectInitializer& initializer) noexcept;
};
