#include "GameModes/SagaGameEndMode.h"
#include "PlayerControllers/SagaGameEndController.h"

AGameEndMode::AGameEndMode(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
{
	PlayerControllerClass = ASagaGameEndController::StaticClass();
}

void
AGameEndMode::BeginPlay()
{
	Super::BeginPlay();
}
