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


		return EBTNodeResult::Failed;
	}
	
	//Make the AI Move
	UAIBlueprintHelperLibrary::SimpleMoveToActor(Controller, Target);

	//Switch the animation to Moving motion


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
		

		return;
	}

	AActor* Target = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));

	if (!IsValid(Target))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		Controller->StopMovement();

		// Switch the animation to Idle
		

		return;
	}

	// Rotate to the target's direction in order to face the target when moving

}

void USagaBTTask_TraceTarget::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

}
