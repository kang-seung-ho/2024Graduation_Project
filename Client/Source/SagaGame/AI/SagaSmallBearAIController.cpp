// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SagaSmallBearAIController.h"

ASagaSmallBearAIController::ASagaSmallBearAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> AITree(TEXT("/Script/AIModule.BehaviorTree'/Game/AI/BT_SmallBear.BT_SmallBear'"));

	if (AITree.Succeeded())
	{
		mAITreeAsset = AITree.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BlackboardData(TEXT("/Script/AIModule.BlackboardData'/Game/AI/BB_SmallBear.BB_SmallBear'"));

	if (BlackboardData.Succeeded())
	{
		mBlackboardAsset = BlackboardData.Object;
	}
}

void ASagaSmallBearAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ASagaSmallBearAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
