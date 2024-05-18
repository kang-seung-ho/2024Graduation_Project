#include "Saga/Level/SagaMainMenuLevel.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaMainMenuLevel::ASagaMainMenuLevel()
noexcept
	: Super()
{
	SetNextLevelName(TEXT("LobbyLevel"));
}

void
ASagaMainMenuLevel::BeginPlay()
{
	Super::BeginPlay();

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	system->OnFailedToConnect.AddDynamic(this, &ASagaMainMenuLevel::OnFailedToConnect);
	system->OnSignedIn.AddDynamic(this, &ASagaMainMenuLevel::OnSignedIn);
}

void
ASagaMainMenuLevel::OnFailedToConnect(ESagaConnectionContract reason)
{
	//GotoNextLevel();
}

void
ASagaMainMenuLevel::OnSignedIn(int32 my_id, const FName& nickname)
{
	GotoNextLevel();
}
