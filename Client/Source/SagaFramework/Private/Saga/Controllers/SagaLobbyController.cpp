#include "Saga/Controllers/SagaLobbyController.h"

void
ASagaLobbyController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeUIOnly UIOnlyInputMode;
	SetInputMode(UIOnlyInputMode);
}
