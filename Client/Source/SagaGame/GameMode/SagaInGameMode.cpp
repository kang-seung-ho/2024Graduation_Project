// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaInGameMode.h"
#include "../Character/SagaPlayableCharacter.h"
#include "../Character/SagaInGamePlayerController.h"

ASagaInGameMode::ASagaInGameMode()
{
	DefaultPawnClass = ASagaPlayableCharacter::StaticClass();
	PlayerControllerClass = ASagaInGamePlayerController::StaticClass();
}

void ASagaInGameMode::BeginPlay()
{
	Super::BeginPlay();
}
