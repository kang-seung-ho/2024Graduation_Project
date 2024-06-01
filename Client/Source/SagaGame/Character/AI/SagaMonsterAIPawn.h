// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../SagaGame/Public/SagaGameInfo.h"
#include "Character/AI/SagaAIPawn.h"
#include "SagaMonsterAIPawn.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API ASagaMonsterAIPawn : public ASagaAIPawn
{
	GENERATED_BODY()

public:
	ASagaMonsterAIPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
};
