#include "AI/SagaMonsterAIPawn.h"
#include "SagaGameInfo.h"
#include "SagaMonsterAIController.h"
#include "AIMonsterAnimInstance.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"

//DO NOT CHANGE THE FILE's ENCODING

ASagaMonsterAIPawn::ASagaMonsterAIPawn()
{
    AIControllerClass = ASagaMonsterAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    mCapsule->SetCollisionProfileName(TEXT("Enemy"));

    mMovement->MaxSpeed = 650.f;
}

void ASagaMonsterAIPawn::ChangeAnim(EAIMonsterAnim Anim)
{
    mMonsterAnim->ChangeAnim(Anim);
}

void ASagaMonsterAIPawn::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority())
    {
        return;
    }

    AutoPossessAI = EAutoPossessAI::Spawned;

    mMonsterAnim = Cast<UAIMonsterAnimInstance>(mAnimInst);
}

void ASagaMonsterAIPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

float ASagaMonsterAIPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    UAISense_Damage::ReportDamageEvent(GetWorld(), this, EventInstigator, DamageAmount, DamageCauser->GetActorLocation(), GetActorLocation());

    AIHP -= DamageAmount;

    if (AIHP <= 0)
    {
        Destroy();
    }

    return DamageAmount;
}

void ASagaMonsterAIPawn::ExecuteAttack()
{
    FVector Start = GetActorLocation() + GetActorForwardVector() * 50.f;
    FVector End = Start + GetActorForwardVector() * 150.f;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    FHitResult HitResult;
    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,
        Start,
        End,
        FQuat::Identity,
        ECC_GameTraceChannel2,
        FCollisionShape::MakeSphere(50.f),
        Params
    );

    // DrawDebugCapsule(GetWorld(), (Start + End) / 2.f, 150.f / 2.f + 50.f / 2.f, 50.f, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), bHit ? FColor::Red : FColor::Green, false, 3.f);

    if (bHit && HitResult.GetActor())
    {
        // Apply damage to the hit actor
        FDamageEvent DamageEvent;
        HitResult.GetActor()->TakeDamage(90.f, DamageEvent, GetController(), this);


    }
}