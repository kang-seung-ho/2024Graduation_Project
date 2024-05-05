// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SagaWeaponData.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Drink,
	Gum,
	SmokeBomb
};

/**
 * 
 */
UCLASS()
class SAGAGAME_API USagaWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "ItemIcon")
	TObjectPtr<UTexture2D> ItemIcon;

public:
	void SetItemType(EItemType Type) { ItemType = Type; }
	void SetWeaponMesh(TObjectPtr<UTexture2D> Mesh) { ItemIcon = Mesh; }
};
