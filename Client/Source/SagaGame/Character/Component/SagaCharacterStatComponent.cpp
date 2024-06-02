// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/SagaCharacterStatComponent.h"

// Sets default values for this component's properties
USagaCharacterStatComponent::USagaCharacterStatComponent()
{
	MaxHp = 100.0f;
	CurrentHp = MaxHp;
}


// Called when the game starts
void USagaCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHp(MaxHp);
	
}

float USagaCharacterStatComponent::ApplyDamage(float Damage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(Damage, 0, Damage);

	SetHp(PrevHp - ActualDamage);

	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("Character is dead"));
	}

	return ActualDamage;
}

void USagaCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);

	OnHpChanged.Broadcast(CurrentHp);
}
