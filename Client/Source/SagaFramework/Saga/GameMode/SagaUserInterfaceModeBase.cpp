#include "Saga/GameMode/SagaUserInterfaceModeBase.h"
#include "Saga/Controller/SagaInterfaceController.h"

ASagaUserInterfaceModeBase::ASagaUserInterfaceModeBase()
	: ASagaGameModeBase()
{
	DefaultPawnClass = nullptr;
	GameStateClass = nullptr;
	PlayerControllerClass = ASagaInterfaceController::StaticClass();
}
