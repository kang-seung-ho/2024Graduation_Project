#include "Saga/GameModes/SagaMainMenuMode.h"
#include "Saga/PlayerControllers/SagaMainMenuController.h"

ASagaMainMenuMode::ASagaMainMenuMode(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
{
	SetControllerClass(ASagaMainMenuController::StaticClass());
}
