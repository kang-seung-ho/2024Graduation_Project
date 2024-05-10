// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/GameEndMode.h"
#include "SagaGame/Character/SagaGameEndController.h"

AGameEndMode::AGameEndMode()
{
	PlayerControllerClass = ASagaGameEndController::StaticClass();
}

void AGameEndMode::BeginPlay()
{
	Super::BeginPlay();
}
