#include "Saga/GameModes/SagaCharacterChoiceMode.h"
#include "Saga/Controllers/SagaInterfaceController.h"

ASagaCharacterChoiceMode::ASagaCharacterChoiceMode()
	: ASagaInterfaceOnlyModeBase()
{
	SetControllerClass(ASagaInterfaceController::StaticClass());
}
