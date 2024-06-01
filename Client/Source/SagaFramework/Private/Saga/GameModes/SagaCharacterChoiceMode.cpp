#include "Saga/GameModes/SagaCharacterChoiceMode.h"
#include "Saga/Controllers/SagaCharacterChoiceController.h"

ASagaCharacterChoiceMode::ASagaCharacterChoiceMode()
	: ASagaInterfaceOnlyModeBase()
{
	SetControllerClass(ASagaCharacterChoiceController::StaticClass());
}
