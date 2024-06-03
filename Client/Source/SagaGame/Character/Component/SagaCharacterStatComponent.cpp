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
	MaxHp = hp;
}

void
USagaCharacterStatComponent::SetCurrentHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);

	if (OnHpChanged.IsBound())
	{
		OnHpChanged.Broadcast(CurrentHp);
	}
}

float
USagaCharacterStatComponent::ApplyDamage(float dmg)
{
	const float actual_dmg = FMath::Max(0, dmg);
	const float remain_hp = CurrentHp - actual_dmg;

	SetCurrentHp(remain_hp);

	if (remain_hp <= KINDA_SMALL_NUMBER)
	{
		if (OnHpZero.IsBound())
		{
			OnHpZero.Broadcast();
		}

		UE_LOG(LogTemp, Log, TEXT("[USagaCharacterStatComponent] Character is dead"));
	}

	return remain_hp;
}

void
USagaCharacterStatComponent::ResetHp()
{
	SetCurrentHp(MaxHp);
}
