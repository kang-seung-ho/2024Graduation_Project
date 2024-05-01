// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SagaWeaponData.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
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
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "WeaponMesh")
	TObjectPtr<UTexture2D> WeaponMesh;

public:
	void SetItemType(EWeaponType Type) { WeaponType = Type; }
	void SetWeaponMesh(TObjectPtr<UTexture2D> Mesh) { WeaponMesh = Mesh; }
};
