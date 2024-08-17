#pragma once
#include "SagaGame.h"
#include "PlayerControllers/SagaBaseCharacterController.h"

#include "SagaGummyBearController.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|System")
class SAGAGAME_API ASagaGummyBearController : public ASagaBaseCharacterController
{
	GENERATED_BODY()

public:
	ASagaGummyBearController(const FObjectInitializer& initializer) noexcept;
};
