// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SagaBearAIController.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API ASagaBearAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASagaBearAIController();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
