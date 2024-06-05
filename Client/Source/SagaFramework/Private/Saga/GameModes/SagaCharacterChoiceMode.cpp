#include "Saga/GameModes/SagaCharacterChoiceMode.h"
#include "Saga/PlayerControllers/SagaCharacterChoiceController.h"

ASagaCharacterChoiceMode::ASagaCharacterChoiceMode()
	: ASagaInterfaceOnlyModeBase()
{
	SetControllerClass(ASagaCharacterChoiceController::StaticClass());
}
