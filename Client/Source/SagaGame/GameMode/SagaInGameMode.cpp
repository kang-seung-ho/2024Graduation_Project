// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaInGameMode.h"
#include "../Character/SagaPlayableCharacter.h"
#include "../Character/SagaInGamePlayerController.h"

ASagaInGameMode::ASagaInGameMode()
{
	//DefaultPawnClass = ASagaPlayableCharacter::StaticClass();
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassRef(TEXT("/Game/PlayerAssets/BP/BP_SagaPlayableCharacter.BP_SagaPlayableCharacter_C"));
	if (PlayerPawnClassRef.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnClassRef.Class;
	}
	PlayerControllerClass = ASagaInGamePlayerController::StaticClass();
}

void ASagaInGameMode::BeginPlay()
{
	Super::BeginPlay();
}
