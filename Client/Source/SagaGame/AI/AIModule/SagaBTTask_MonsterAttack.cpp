#include "AI/AIModule/SagaBTTask_MonsterAttack.h"
#include "../SagaMonsterAIPawn.h"
#include "../SagaMonsterAIController.h"
#include "Character/SagaPlayableCharacter.h"

USagaBTTask_MonsterAttack::USagaBTTask_MonsterAttack()
{
    NodeName = TEXT("MonsterAttack");
    bNotifyTick = true;
    bNotifyTaskFinished = true;
}

EBTNodeResult::Type USagaBTTask_MonsterAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    // Get the AI controller which is working now
    AAIController* Controller = OwnerComp.GetAIOwner();

    // Get the AI pawn which is controlled by the AI controller
    ASagaMonsterAIPawn* Monster = Controller->GetPawn<ASagaMonsterAIPawn>();

    if (!IsValid(Monster))
    {
        return EBTNodeResult::Failed;
    }

    // Get the Target which is saved in the Blackboard
    AActor* Target = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
    if (!IsValid(Target))
    {
        // Stop the AI's movement
        Controller->StopMovement();

        // Switch the animation to IDLE
        Monster->ChangeAnim(EAIMonsterAnim::Idle);

        return EBTNodeResult::Failed;
    }

    // Change the animation to Attack
    if (IsValid(Target) && Target->IsA(ASagaPlayableCharacter::StaticClass()))
    {
        Monster->ChangeAnim(EAIMonsterAnim::Attack);
    }


    return EBTNodeResult::InProgress;
}

void USagaBTTask_MonsterAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* Controller = OwnerComp.GetAIOwner();
    ASagaMonsterAIPawn* Monster = Controller->GetPawn<ASagaMonsterAIPawn>();

    if (!IsValid(Monster))
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        Controller->StopMovement();
        Monster->ChangeAnim(EAIMonsterAnim::Idle);
        return;
    }

    AActor* Target = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
    if (!IsValid(Target))
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        Controller->StopMovement();
        Monster->ChangeAnim(EAIMonsterAnim::Idle);
        return;
    }

    // Check if the attack animation has ended
    if (Monster->GetAttackAnimEnd())
    {
        Monster->SetAttackAnimEnd(false);

        FVector TargetLocation = Target->GetActorLocation();
        FVector MonsterLocation = Monster->GetActorLocation();

        MonsterLocation.Z -= Monster->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
        UCapsuleComponent* TargetCapsule = Cast<UCapsuleComponent>(Target->GetRootComponent());
        if (IsValid(TargetCapsule))
        {
            TargetLocation.Z -= TargetCapsule->GetScaledCapsuleHalfHeight();
        }

        float Distance = FVector::Distance(MonsterLocation, TargetLocation);

        // Target out of range. End the Attack.
        if (Distance > 150.f)
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            Controller->StopMovement();
            Monster->ChangeAnim(EAIMonsterAnim::Idle);
        }
        else
        {
            // If target is within range, continue attacking
            Monster->ChangeAnim(EAIMonsterAnim::Attack);

            TargetLocation.Z = 0.0;
            MonsterLocation.Z = 0.0;

            FVector Dir = TargetLocation - MonsterLocation;
            Dir.Normalize();

            FRotator DirRot = Dir.Rotation();
            Monster->SetActorRotation(FRotator(0.0, DirRot.Yaw, 0.0));
        }
    }
}

void USagaBTTask_MonsterAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
