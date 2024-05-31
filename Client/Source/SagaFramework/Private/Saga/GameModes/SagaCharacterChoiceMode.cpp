#include "Saga/GameModes/SagaCharacterChoiceMode.h"
#include "SagaGame/Character/CharacterSelect/SagaCharacterSelectController.h"

ASagaCharacterChoiceMode::ASagaCharacterChoiceMode()
	: ASagaInterfaceOnlyModeBase()
{
	SetControllerClass(ASagaCharacterSelectController::StaticClass());
}
