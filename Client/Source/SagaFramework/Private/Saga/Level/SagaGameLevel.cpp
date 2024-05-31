#include "Saga/Level/SagaGameLevel.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaGameLevel::ASagaGameLevel()
noexcept
	: Super()
{
	SetNextLevelName(TEXT("GameVictoryLevel"));
	SetPrevLevelName(TEXT("MainMenuLevel"));
}

void
ASagaGameLevel::BeginPlay()
{
	Super::BeginPlay();

	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	system->OnDisconnected.AddDynamic(this, &ASagaGameLevel::OnDisconnected);
}

void
ASagaGameLevel::BeginDestroy()
{
	Super::BeginDestroy();
}

void
ASagaGameLevel::OnDisconnected()
{
	GotoPrevLevel();
}
