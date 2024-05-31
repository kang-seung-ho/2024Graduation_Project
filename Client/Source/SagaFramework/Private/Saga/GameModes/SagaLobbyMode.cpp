#include "Saga/GameModes/SagaLobbyMode.h"
#include "Saga/Controllers/SagaLobbyController.h"

ASagaLobbyMode::ASagaLobbyMode()
	: ASagaInterfaceOnlyModeBase()
{
	SetControllerClass(ASagaLobbyController::StaticClass());
}
