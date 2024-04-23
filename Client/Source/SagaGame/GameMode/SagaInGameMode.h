// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../SagaGameInfo.h"
#include "GameFramework/GameModeBase.h"
#include "SagaInGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API ASagaInGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ASagaInGameMode();

protected:
	virtual void BeginPlay() override;
};
