// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/SagaItemData.h"
#include "SagaWeaponItemData.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API USagaWeaponItemData : public USagaItemData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = Weapon)
	TSoftObjectPtr<USkeletalMesh> WeaponMesh; //메시는 블루프린트에서 지정해준다.


};
