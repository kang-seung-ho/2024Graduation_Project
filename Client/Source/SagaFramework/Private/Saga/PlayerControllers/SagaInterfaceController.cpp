#include "Saga/PlayerControllers/SagaInterfaceController.h"

void
ASagaInterfaceController::BeginPlay()
{
	Super::BeginPlay();

	SetShowMouseCursor(true);
}
