#include "Saga/GameModes/SagaLobbyMode.h"
#include "Saga/PlayerControllers/SagaLobbyController.h"

ASagaLobbyMode::ASagaLobbyMode()
	: ASagaInterfaceOnlyModeBase()
{
	SetControllerClass(ASagaLobbyController::StaticClass());
}
