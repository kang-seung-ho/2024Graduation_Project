#include "AIPawn/SagaMonsterAIPawn.h"
#include "AIPawn/SagaBearAIController.h"

ASagaMonsterAIPawn::ASagaMonsterAIPawn()
{
	AIControllerClass = ASagaBearAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASagaMonsterAIPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ASagaMonsterAIPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
