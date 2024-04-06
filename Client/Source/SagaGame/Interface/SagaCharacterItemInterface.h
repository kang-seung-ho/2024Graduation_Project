// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SagaCharacterItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USagaCharacterItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SAGAGAME_API ISagaCharacterItemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void TakeItem(class USagaItemData* InItemData) = 0;
};
