#include "AI/SagaMonsterAIPawn.h"
#include "SagaGameInfo.h"
#include "SagaMonsterAIController.h"

//DO NOT CHANGE THE FILE's ENCODING

ASagaMonsterAIPawn::ASagaMonsterAIPawn()
{
	AIControllerClass = ASagaMonsterAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
       

    mCapsule->SetCollisionProfileName(TEXT("Enemy"));

    mMovement->MaxSpeed = 400.f;
}

void ASagaMonsterAIPawn::BeginPlay()
{
	Super::BeginPlay();

    if (!HasAuthority())
    {
        return;
    }

    
    AutoPossessAI = EAutoPossessAI::Spawned;


    /*if (AIControllerClass != nullptr && GetController() == nullptr)
    {
        SpawnDefaultController();
    }*/
}

void ASagaMonsterAIPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ASagaMonsterAIPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    UAISense_Damage::ReportDamageEvent(GetWorld(), this, EventInstigator, DamageAmount, DamageCauser->GetActorLocation(), GetActorLocation());

    /*DetachFromControllerPendingDestroy();

    Destroy();*/

    return DamageAmount;
}