// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../SagaGameInfo.h"
#include "GameFramework/PlayerController.h"
#include "SagaCharacterSelectController.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API ASagaCharacterSelectController : public APlayerController
{
	GENERATED_BODY()

public:
	ASagaCharacterSelectController();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
