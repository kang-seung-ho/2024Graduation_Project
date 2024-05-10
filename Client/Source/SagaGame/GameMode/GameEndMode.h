// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameEndMode.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API AGameEndMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameEndMode();
	virtual void BeginPlay() override;
	
};
