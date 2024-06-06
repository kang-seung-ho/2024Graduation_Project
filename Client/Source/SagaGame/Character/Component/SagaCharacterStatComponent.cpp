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

	SetActive(true, true);
}

void
USagaCharacterStatComponent::SetMaxHp(float maxhp, ESagaMaxHealthUpdatePolicy current_health_policy)
{
	switch (current_health_policy)
	{
	case ESagaMaxHealthUpdatePolicy::Ignore:
	{}
	break;

	case ESagaMaxHealthUpdatePolicy::SetCurrentHealthFromRatio:
	{
		const auto pre_ratio = CurrentHp / MaxHp;

		CurrentHp = maxhp * pre_ratio;
	}
	break;

	case ESagaMaxHealthUpdatePolicy::SetCurrentHealthToMaxHealth:
	{
		CurrentHp = maxhp;
	}
	break;

	default:
	{
		CurrentHp = maxhp;
	}
	break;
	}

	MaxHp = maxhp;
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
	}

	return remain_hp;
}

void
USagaCharacterStatComponent::ResetHp(bool execute_event)
{
	if (execute_event)
	{
		SetCurrentHp(MaxHp);
	}
	else
	{
		CurrentHp = MaxHp;
	}
}
