// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SagaCharacterPlayer.h"
#include "SagaPlayableCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API ASagaPlayableCharacter : public ASagaCharacterPlayer
{
	GENERATED_BODY()
	
public:
	ASagaPlayableCharacter();


protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
