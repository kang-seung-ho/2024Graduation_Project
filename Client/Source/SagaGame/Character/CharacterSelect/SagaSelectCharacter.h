// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../SagaGameInfo.h"
#include "GameFramework/Actor.h"
#include "SagaSelectCharacter.generated.h"

UENUM(BlueprintType)
enum class EPlayerWeapon : uint8
{
	None,
	LightSabor,
	WaterGun,
	Hammer
};


UCLASS()
class SAGAGAME_API ASagaSelectCharacter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASagaSelectCharacter();

protected:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* mBody;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* mMesh;

	UPROPERTY(EditAnywhere)
	EPlayerWeapon mWeapon = EPlayerWeapon::None;

public:
	EPlayerWeapon GetWeapon() const
	{
		return mWeapon; 
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
