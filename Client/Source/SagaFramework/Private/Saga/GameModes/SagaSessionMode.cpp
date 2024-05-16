#include "Saga/GameModes/SagaSessionMode.h"
#include "Saga/Controllers/SagaSessionController.h"

ASagaSessionMode::ASagaSessionMode()
	: ASagaInterfaceOnlyModeBase()
{
	SetControllerClass(ASagaSessionController::StaticClass());
}
