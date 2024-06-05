#include "Saga/GameModes/SagaInterfaceOnlyModeBase.h"

ASagaInterfaceOnlyModeBase::ASagaInterfaceOnlyModeBase(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
{
	SetPawnClass(nullptr);
	SetSpectatorClass(nullptr);
	SetSessionClass(nullptr);
	SetHUDClass(nullptr);
}
