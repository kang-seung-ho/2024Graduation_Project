#include "Saga/GameModes/SagaSessionMode.h"
#include "Saga/PlayerControllers/SagaSessionController.h"

ASagaSessionMode::ASagaSessionMode(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
{
	SetControllerClass(ASagaSessionController::StaticClass());
}
