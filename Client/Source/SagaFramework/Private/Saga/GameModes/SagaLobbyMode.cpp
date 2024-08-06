#include "Saga/GameModes/SagaLobbyMode.h"
#include "Saga/PlayerControllers/SagaLobbyController.h"

ASagaLobbyMode::ASagaLobbyMode(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
{
	SetControllerClass(ASagaLobbyController::StaticClass());
}
