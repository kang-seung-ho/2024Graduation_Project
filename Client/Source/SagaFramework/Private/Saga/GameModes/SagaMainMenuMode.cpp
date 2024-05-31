#include "Saga/GameModes/SagaMainMenuMode.h"
#include "Saga/Controllers/SagaMainMenuController.h"

ASagaMainMenuMode::ASagaMainMenuMode()
	: ASagaInterfaceOnlyModeBase()
{
	SetControllerClass(ASagaMainMenuController::StaticClass());
}
