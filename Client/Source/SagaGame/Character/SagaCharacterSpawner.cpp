#include "Character/SagaCharacterSpawner.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaCharacterSpawner::ASagaCharacterSpawner()
	: spawnCount()
	, awaitingPlayers()
{
	PrimaryActorTick.bCanEverTick = true;
}

FVector
ASagaCharacterSpawner::GetInitialSpawnPoint()
{
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	const auto my_pos = GetActorLocation();

	return FVector
	{
		my_pos.X,
		my_pos.Y + (spawnCount++) * 70.0,
		my_pos.Z,
	};
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
