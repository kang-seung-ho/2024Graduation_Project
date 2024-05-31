#include "Saga/GameModes/SagaInterfaceOnlyModeBase.h"

ASagaInterfaceOnlyModeBase::ASagaInterfaceOnlyModeBase()
	: Super()
{
	SetPawnClass(nullptr);
	SetSpectatorClass(nullptr);
	SetSessionClass(nullptr);
	SetHUDClass(nullptr);
}

void
ASagaInterfaceOnlyModeBase::SetControllerClass(TSubclassOf<APlayerController> controller_class)
noexcept
{
	PlayerControllerClass = controller_class;
}

void
ASagaInterfaceOnlyModeBase::SetPawnClass(TSubclassOf<APawn> pawn_class)
noexcept
{
	GameSessionClass = pawn_class;
}

void
ASagaInterfaceOnlyModeBase::SetSpectatorClass(TSubclassOf<ASpectatorPawn> pawn_class)
noexcept
{
	DefaultPawnClass = pawn_class;
}

void
ASagaInterfaceOnlyModeBase::SetSessionClass(TSubclassOf<AGameSession> session_class)
noexcept
{
	SpectatorClass = session_class;
}

void
ASagaInterfaceOnlyModeBase::SetHUDClass(TSubclassOf<AHUD> hud_class)
noexcept
{
	HUDClass = hud_class;
}
