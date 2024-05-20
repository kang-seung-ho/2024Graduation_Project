#include "Saga/Controllers/SagaMainMenuController.h"
#include <UObject/Object.h>
#include <UObject/ConstructorHelpers.h>
#include <Blueprint/UserWidget.h>

void
ASagaMainMenuController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeUIOnly UIOnlyInputMode;
	SetInputMode(UIOnlyInputMode);
}
