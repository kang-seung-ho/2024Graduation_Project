#include "Saga/GameModes/SagaCharacterChoiceMode.h"
#include "Saga/PlayerControllers/SagaCharacterChoiceController.h"

ASagaCharacterChoiceMode::ASagaCharacterChoiceMode(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
{
	SetControllerClass(ASagaCharacterChoiceController::StaticClass());
}
