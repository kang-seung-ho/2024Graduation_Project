#include "Saga/GameModes/SagaSessionMode.h"
#include "Saga/PlayerControllers/SagaSessionController.h"

ASagaSessionMode::ASagaSessionMode()
	: ASagaInterfaceOnlyModeBase()
{
	SetControllerClass(ASagaSessionController::StaticClass());
}
