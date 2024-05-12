#include "Character/SagaCharacterSpawner.h"

ASagaCharacterSpawner::ASagaCharacterSpawner()
	: awaitingPlayers()
{
	PrimaryActorTick.bCanEverTick = true;
}

void
ASagaCharacterSpawner::BeginPlay()
{
	Super::BeginPlay();

}

void
ASagaCharacterSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
