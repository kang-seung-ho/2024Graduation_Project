// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Items.h"
#include "EnergyDrinkItem.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API UEnergyDrinkItem : public UItems
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item, meta = (ClampMin = 0.0))
	float HealAmount;
	
protected:

	virtual void Use(class ASagaCharacterPlayer* Character) override;
};
