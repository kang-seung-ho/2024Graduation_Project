// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../SagaAIInfo.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SagaBTTask_TraceTarget.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API USagaBTTask_TraceTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	USagaBTTask_TraceTarget();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);
};
