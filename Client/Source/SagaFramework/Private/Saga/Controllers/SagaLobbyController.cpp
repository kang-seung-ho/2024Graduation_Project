#include "Saga/Controllers/SagaLobbyController.h"

void
ASagaLobbyController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeUIOnly mode{};
	SetInputMode(mode);
}
