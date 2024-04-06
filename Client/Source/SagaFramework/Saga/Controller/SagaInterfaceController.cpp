#include "Saga/Controller/SagaInterfaceController.h"

void
ASagaInterfaceController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeUIOnly UIOnlyInputMode;
    SetInputMode(UIOnlyInputMode);

	SetShowMouseCursor(true);
}

void
ASagaInterfaceController::SetupInputComponent()
{
	Super::SetupInputComponent();
}
