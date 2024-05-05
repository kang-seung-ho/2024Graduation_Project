// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SagaWeaponData.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Hammer,
	Lightsaber,
	WaterGun
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

	UPROPERTY(EditAnywhere, Category = "WeaponMesh")
	TObjectPtr<UTexture2D> WeaponMesh;

public:
	void SetItemType(EItemType Type) { ItemType = Type; }
	void SetWeaponMesh(TObjectPtr<UTexture2D> Mesh) { WeaponMesh = Mesh; }
};
