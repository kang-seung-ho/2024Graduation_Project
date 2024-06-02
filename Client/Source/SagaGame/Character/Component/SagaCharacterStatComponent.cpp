#include "Character/Component/SagaCharacterStatComponent.h"

USagaCharacterStatComponent::USagaCharacterStatComponent(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, MaxHp(defaultMaxHealth), CurrentHp(defaultMaxHealth)
	, OnHpZero(), OnHpChanged()
{}

void
USagaCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void
USagaCharacterStatComponent::SetMaxHp(float hp, ESagaMaxHealthUpdatePolicy current_health_policy)
{

}

void
USagaCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);

	if (OnHpChanged.IsBound())
	{
		OnHpChanged.Broadcast(CurrentHp);
	}
}

float
USagaCharacterStatComponent::ApplyDamage(float Damage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(Damage, 0, Damage);

	SetHp(PrevHp - ActualDamage);

	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		if (OnHpZero.IsBound())
		{
			OnHpZero.Broadcast();
		}

		UE_LOG(LogTemp, Log, TEXT("[USagaCharacterStatComponent] Character is dead"));
	}

	return ActualDamage;
}
