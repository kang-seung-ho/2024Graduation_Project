// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/SagaMonsterAIController.h"
#include "SagaSmallBearAIController.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API ASagaSmallBearAIController : public ASagaMonsterAIController
{
	GENERATED_BODY()
	
public:
	ASagaSmallBearAIController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
