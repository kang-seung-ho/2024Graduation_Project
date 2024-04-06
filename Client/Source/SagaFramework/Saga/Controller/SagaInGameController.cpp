#include "Saga/Controller/SagaInGameController.h"

void
ASagaInGameController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	SetShowMouseCursor(false);
}

void
ASagaInGameController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Jump 액션에 대한 입력 바인딩
	// InputComponent->BindAction("Jump", IE_Pressed, this,
	// &ASagaPlayerController::Jump);
}
