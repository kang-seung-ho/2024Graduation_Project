#include "SagaCharacterSelectGameMode.h"
#include "SagaCharacterSelectController.h"

ASagaCharacterSelectGameMode::ASagaCharacterSelectGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ASagaCharacterSelectController::StaticClass();
}
