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
USagaCharacterStatComponent::SetCurrentHp(float hp)
{
	CurrentHp = FMath::Clamp<float>(hp, 0.0f, MaxHp);

	if (OnHpChanged.IsBound())
	{
		OnHpChanged.Broadcast(CurrentHp / MaxHp);
	}
}

float
USagaCharacterStatComponent::ApplyDamage(float dmg)
{
	const float actual_dmg = FMath::Max(0, dmg);

	const auto prior_hp = CurrentHp;
	const float remain_hp = prior_hp - actual_dmg;

	SetCurrentHp(remain_hp);

	if (0 < prior_hp and remain_hp <= KINDA_SMALL_NUMBER)
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

void
USagaCharacterStatComponent::RetryUpdateHealth()
const
{
	if (OnHpChanged.IsBound())
	{
		OnHpChanged.Broadcast(CurrentHp / MaxHp);
	}
}
