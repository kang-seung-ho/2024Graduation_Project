#include "Saga/GameModes/SagaMainMenuMode.h"
#include "Saga/PlayerControllers/SagaMainMenuController.h"

ASagaMainMenuMode::ASagaMainMenuMode()
	: ASagaInterfaceOnlyModeBase()
{
	SetControllerClass(ASagaMainMenuController::StaticClass());
}
