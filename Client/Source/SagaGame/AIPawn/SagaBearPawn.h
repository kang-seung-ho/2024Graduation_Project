// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SagaMonsterAIPawn.h"
#include "SagaBearPawn.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API ASagaBearPawn : public ASagaMonsterAIPawn
{
	GENERATED_BODY()
	
public:
	// Sets default values for this pawn's properties
	ASagaBearPawn();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
