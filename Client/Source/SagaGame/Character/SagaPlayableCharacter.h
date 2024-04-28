// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SagaCharacterPlayer.h"
#include "../Item/SagaWeaponData.h"
#include "../Interface/SagaCharacterItemInterface.h"
#include "SagaPlayableCharacter.generated.h"


DECLARE_DELEGATE_OneParam(FOnTakeWeaponDelegate, class USagaWeaponData*);
USTRUCT(BlueprintType)
struct FTakeWeaponDelegateWrapper
{
	GENERATED_BODY()
	FTakeWeaponDelegateWrapper() {}
	FTakeWeaponDelegateWrapper(const FOnTakeWeaponDelegate& InWeaponDelegate) : WeaponDelegate(InWeaponDelegate) {}

	FOnTakeWeaponDelegate WeaponDelegate;
};
/**
 * 
 */
UCLASS()
class SAGAGAME_API ASagaPlayableCharacter : public ASagaCharacterPlayer, public ISagaCharacterItemInterface
{
	GENERATED_BODY()
	
public:
	ASagaPlayableCharacter();

protected:
	bool isRiding = false;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Weapon;


protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EWeaponType mWeaponType;

public:
	EWeaponType GetWeaponType() const { return mWeaponType; }

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY()
	TArray<FTakeWeaponDelegateWrapper> TakeWeaponAction;
	virtual void EquipHammer(class USagaWeaponData* WeaponData);
	virtual void EquipLightSaber(class USagaWeaponData* WeaponData);
	virtual void EquipWaterGun(class USagaWeaponData* WeaponData);

	virtual void TakeItem(class USagaWeaponData* WeaponData) override;
};
