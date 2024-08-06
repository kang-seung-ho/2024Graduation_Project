#include "Saga/GameModes/SagaGameModeBase.h"

ASagaGameModeBase::ASagaGameModeBase(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
{
	SetHUDClass(nullptr);
}

void
ASagaGameModeBase::SetControllerClass(TSubclassOf<APlayerController> controller_class)
noexcept
{
	PlayerControllerClass = controller_class;
}

void
ASagaGameModeBase::SetPawnClass(TSubclassOf<APawn> pawn_class)
noexcept
{
	DefaultPawnClass = pawn_class;
}

void
ASagaGameModeBase::SetSpectatorClass(TSubclassOf<ASpectatorPawn> pawn_class)
noexcept
{
	SpectatorClass = pawn_class;
}

void
ASagaGameModeBase::SetSessionClass(TSubclassOf<AGameSession> session_class)
noexcept
{
	GameSessionClass = session_class;
}

void
ASagaGameModeBase::SetHUDClass(TSubclassOf<AHUD> hud_class)
noexcept
{
	HUDClass = hud_class;
}
