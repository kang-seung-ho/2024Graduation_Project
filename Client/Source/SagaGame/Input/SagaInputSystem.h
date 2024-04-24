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
	UInputAction* Move = nullptr;
	UInputAction* Attack = nullptr;
};