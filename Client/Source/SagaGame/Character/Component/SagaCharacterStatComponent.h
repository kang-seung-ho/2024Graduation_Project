// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SagaCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZero);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChanged, float);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SAGAGAME_API USagaCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USagaCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	FOnHpZero OnHpZero;
	FOnHpChanged OnHpChanged;

	FORCEINLINE float GetMaxHp() const { return MaxHp; }
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }
	float ApplyDamage(float Damage);

protected:
	void SetHp(float NewHp);

	UPROPERTY(VisibleInstanceOnly, Category = "Character Stat")
	float MaxHp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Character Stat")
	float CurrentHp;


		
};
