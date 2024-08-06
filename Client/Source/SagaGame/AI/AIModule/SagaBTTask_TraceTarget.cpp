// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIModule/SagaBTTask_TraceTarget.h"
#include "../SagaMonsterAIPawn.h"
#include "../SagaMonsterAIController.h"

USagaBTTask_TraceTarget::USagaBTTask_TraceTarget()
{
	NodeName = TEXT("TraceTarget");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type USagaBTTask_TraceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Retval = Super::ExecuteTask(OwnerComp, NodeMemory);

	// Get the AI controller which is working now
	AAIController* Controller = OwnerComp.GetAIOwner();

	// Get the AI pawn which is controlled by the AI controller
	ASagaMonsterAIPawn* Monster = Controller->GetPawn<ASagaMonsterAIPawn>();

	if(!IsValid(Monster))
	{
		return EBTNodeResult::Failed;
	}

	//Get the Target which is saved in the Blackboard
	AActor* Target = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	if (!IsValid(Target))
	{
		//Stop the AI's movement
		Controller->StopMovement();

		//switch the animation to IDLE
		Monster->ChangeAnim(EAIMonsterAnim::Idle);

		return EBTNodeResult::Failed;
	}
	
	//Make the AI Move
	UAIBlueprintHelperLibrary::SimpleMoveToActor(Controller, Target);

	//Switch the animation to Moving motion
	Monster->ChangeAnim(EAIMonsterAnim::Run);

	// The task is not completed until the AI reaches the target
	// So it has to keep checking whether the AI reaches the target or not.
	// In order to do that, Make the state to "InProgress"
	return EBTNodeResult::InProgress;
}

void USagaBTTask_TraceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* Controller = OwnerComp.GetAIOwner();

	ASagaMonsterAIPawn* Monster = Controller->GetPawn<ASagaMonsterAIPawn>();

	if (!IsValid(Monster))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		Controller->StopMovement();

		// Switch the animation to Idle.
		Monster->ChangeAnim(EAIMonsterAnim::Idle);

		return;
	}

	AActor* Target = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));

	if (!IsValid(Target))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		Controller->StopMovement();

		// Switch the animation to Idle
		Monster->ChangeAnim(EAIMonsterAnim::Idle);

		return;
	}

	// Rotate to the target's direction in order to face the target when moving
	// Get the direction to the target
	//FVector TargetLocation = Target->GetActorLocation();
	//FVector MonsterLocation = Monster->GetActorLocation();

	////using the speed vector
	////FVector Velocity = Monster->GetMovementComponent()->Velocity;

	//TargetLocation.Z = 0.0;
	//MonsterLocation.Z = 0.0;

	//FVector Dir = TargetLocation - MonsterLocation;
	//// Normalize the direction
	//Dir.Normalize();

	//// Get the rotation / only Yaw value
	//FRotator DirRot = Dir.Rotation();

	//Monster->SetActorRotation(FRotator(0.0, DirRot.Yaw, 0.0));

	// Check whether the AI has reached the target
	FVector TargetLocation = Target->GetActorLocation();
	FVector MonsterLocation = Monster->GetActorLocation();

	MonsterLocation.Z -= Monster->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	UCapsuleComponent* TargetCapsule = Cast<UCapsuleComponent>(Target->GetRootComponent());
	if (IsValid(TargetCapsule))
	{
		TargetLocation.Z -= TargetCapsule->GetScaledCapsuleHalfHeight();
	}

	float Distance = FVector::Distance(MonsterLocation, TargetLocation);

	// If the AI reaches the attack range, start the attack
	if (Distance <= 150.f)
	{
		//The attack will only work if the state is failure.
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

		Controller->StopMovement();

	}

}

void USagaBTTask_TraceTarget::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);


}
