// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaCharacterSelectGameMode.h"
#include "SagaCharacterSelectController.h"

ASagaCharacterSelectGameMode::ASagaCharacterSelectGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ASagaCharacterSelectController::StaticClass();
}
