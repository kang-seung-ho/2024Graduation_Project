// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/SagaMonsterAIPawn.h"
#include "GameFramework/Character.h"
#include "SagaGummyBearSmall.generated.h"

UCLASS()
class SAGAGAME_API ASagaGummyBearSmall : public ASagaMonsterAIPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASagaGummyBearSmall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
