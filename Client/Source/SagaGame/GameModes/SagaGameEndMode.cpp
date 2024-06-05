#include "GameModes/SagaGameEndMode.h"
#include "PlayerControllers/SagaGameEndController.h"

AGameEndMode::AGameEndMode()
	: Super()
{
	PlayerControllerClass = ASagaGameEndController::StaticClass();
}

void
AGameEndMode::BeginPlay()
{
	Super::BeginPlay();
}
