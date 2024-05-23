#include "Saga/Level/SagaCharacterChoiceLevel.h"

ASagaCharacterChoiceLevel::ASagaCharacterChoiceLevel()
noexcept
	: Super()
{
	SetNextLevelName(TEXT("SagaGameLevel"));
	SetPrevLevelName(TEXT("MainMenuLevel"));
}
