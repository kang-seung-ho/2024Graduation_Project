#include "Saga/GameModes/SagaInterfaceOnlyModeBase.h"

ASagaInterfaceOnlyModeBase::ASagaInterfaceOnlyModeBase()
noexcept
	: Super()
{
	SetPawnClass(nullptr);
	SetSpectatorClass(nullptr);
	SetSessionClass(nullptr);
	SetHUDClass(nullptr);
}
