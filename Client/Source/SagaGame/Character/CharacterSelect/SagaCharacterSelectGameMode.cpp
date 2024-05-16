#include "SagaCharacterSelectGameMode.h"
#include "SagaGameInfo.h"
#include "SagaCharacterSelectController.h"

ASagaCharacterSelectGameMode::ASagaCharacterSelectGameMode()
	: AGameModeBase()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ASagaCharacterSelectController::StaticClass();
}
