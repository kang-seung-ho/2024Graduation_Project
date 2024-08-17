#pragma once
#include "SagaGame.h"
#include "PlayerControllers/SagaBaseCharacterController.h"

#include "SagaInGameRemotePlayerController.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|System")
class SAGAGAME_API ASagaInGameRemotePlayerController : public ASagaBaseCharacterController
{
	GENERATED_BODY()

public:
	ASagaInGameRemotePlayerController(const FObjectInitializer& initializer) noexcept;
};
