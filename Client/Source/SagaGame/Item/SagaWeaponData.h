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
	EItemType WeaponType;

	UPROPERTY(EditAnywhere, Category = "WeaponMesh")
	TObjectPtr<UStaticMesh> WeaponMesh;

public:
	void SetItemType(EItemType Type) { WeaponType = Type; }
	void SetWeaponMesh(TObjectPtr<UStaticMesh> Mesh) { WeaponMesh = Mesh; }
};
