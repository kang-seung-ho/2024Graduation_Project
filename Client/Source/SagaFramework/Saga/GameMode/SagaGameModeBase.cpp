#include "Saga/GameMode/SagaGameModeBase.h"

ASagaGameModeBase::ASagaGameModeBase()
	: AGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}
