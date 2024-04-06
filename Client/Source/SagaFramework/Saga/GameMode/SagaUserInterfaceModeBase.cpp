#include "Saga/GameMode/SagaUserInterfaceModeBase.h"
#include "Saga/Controller/SagaInterfaceController.h"

ASagaUserInterfaceModeBase::ASagaUserInterfaceModeBase()
	: ASagaGameModeBase()
{
	PlayerControllerClass = ASagaInterfaceController::StaticClass();
}
