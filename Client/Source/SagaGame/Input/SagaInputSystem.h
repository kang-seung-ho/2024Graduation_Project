// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "UObject/NoExportTypes.h"
#include "SagaInputSystem.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API USagaInputSystem : public UObject
{
	GENERATED_BODY()
	
public:
	USagaInputSystem();

public:
	UInputMappingContext* DefaultContext = nullptr;

public:
	UInputAction* ForwardBackMove = nullptr;
	UInputAction* StrafeMove = nullptr; // Left and Right
	UInputAction* Attack = nullptr;
	UInputAction* Jump = nullptr;
	UInputAction* Rotate = nullptr;
	UInputAction* Sprint = nullptr;
	UInputAction* Interact = nullptr;
	UInputAction* Activate = nullptr;
	UInputAction* Skill1 = nullptr;
	UInputAction* Skill2 = nullptr;
	UInputAction* Skill3 = nullptr;
	UInputAction* Skill4 = nullptr;
	UInputAction* Item1 = nullptr;
	UInputAction* Item2 = nullptr;
	UInputAction* Item3 = nullptr;
};


UCLASS()
class SAGAGAME_API USagaCharacterSelectInputSystem : public UObject
{
	GENERATED_BODY()

public:
	USagaCharacterSelectInputSystem();

public:
	UInputMappingContext* DefaultContext = nullptr;

public:
	UInputAction* Click = nullptr;
};
