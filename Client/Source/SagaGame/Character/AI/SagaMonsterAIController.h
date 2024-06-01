// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../SagaGame/Public/SagaGameInfo.h"
#include "AIController.h"
#include "SagaMonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API ASagaMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASagaMonsterAIController();

	void RunAI();
	void StopAI();

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	TObjectPtr<class UBlackboardData> BBAsset;

	UPROPERTY()
	TObjectPtr<class UBehaviorTree> BTAsset;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
};
