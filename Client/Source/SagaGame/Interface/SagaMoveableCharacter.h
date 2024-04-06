// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputActionValue.h"
#include "SagaMoveableCharacter.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USagaMoveableCharacter : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SAGAGAME_API ISagaMoveableCharacter
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual void StartShoulderMove(const FInputActionValue& Value) = 0;
	virtual void ShoulderLook(const FInputActionValue& Value) = 0;

	virtual void StartAttack() = 0;

};
