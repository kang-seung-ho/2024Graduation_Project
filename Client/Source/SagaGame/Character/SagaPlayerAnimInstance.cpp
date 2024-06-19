#include "Character/SagaPlayerAnimInstance.h"
#include <UObject/Object.h>

#include "Character/SagaPlayableCharacter.h"

void
USagaPlayerAnimInstance::PlayAttackMontage()
{
	if (!mAttackEnable)
		return;

	mAttackEnable = false;

	if (!Montage_IsPlaying(mAttackMontageArray[mAttackIndex]))
	{
		if (mWeaponTypes == EPlayerWeapon::LightSabor)
		{
			Montage_SetPosition(mAttackMontageArray[mAttackIndex], 0.f);

			Montage_Play(mAttackMontageArray[mAttackIndex]);

			mAttackIndex = (mAttackIndex + 1) % mAttackMontageArray.Num();
		}
		else if (mWeaponTypes == EPlayerWeapon::WaterGun)
		{
			Montage_SetPosition(mGunAttackMontageArray[mAttackIndex], 0.f);

			Montage_Play(mGunAttackMontageArray[mAttackIndex]);

			mAttackIndex = (mAttackIndex + 1) % mGunAttackMontageArray.Num();
		}
		else if (mWeaponTypes == EPlayerWeapon::Hammer)
		{
			Montage_SetPosition(mAttackMontageArray[mAttackIndex], 0.f);

			Montage_Play(mAttackMontageArray[mAttackIndex]);

			mAttackIndex = (mAttackIndex + 1) % mAttackMontageArray.Num();
		}
	}
}

void USagaPlayerAnimInstance::Death()
{
	mIsDead = true;
}

void USagaPlayerAnimInstance::Hit()
{
	mAttackEnable = true;

	mHitRecovery = 1.f;

	if (!Montage_IsPlaying(mHitMontage))
	{
		//Montage_SetPosition(mHitMontage, 0.f);

		Montage_Play(mHitMontage);
	}
}

void USagaPlayerAnimInstance::Revive()
{
	mIsDead = false;
	mHitRecovery = 0.f;
}

void
USagaPlayerAnimInstance::AnimNotify_Attack()
{
	const auto PlayerCharacter = GetPawnOwner();

	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->ExecuteAttack();
	}
}

void
USagaPlayerAnimInstance::AnimNotify_AttackEnable()
{
	mAttackEnable = true;
}

void
USagaPlayerAnimInstance::AnimNotify_AttackEnd()
{
	mAttackIndex = 0;
}

void USagaPlayerAnimInstance::AnimNotify_HitRecovery()
{
	mHitRecovery = 0.f;
}

ASagaPlayableCharacter*
USagaPlayerAnimInstance::GetPawnOwner()
const
{
	return Cast<ASagaPlayableCharacter>(TryGetPawnOwner());
}

void
USagaPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	mMoveSpeed = 0.f;
	mMoveDir = 0.f;
}

void
USagaPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const auto character = GetPawnOwner();

	if (IsValid(character))
	{
		mWeaponTypes = character->GetWeapon();

		mMoveSpeed = character->GetMoveAnimationSpeed();
		mMoveDir = character->GetMoveAnimationAngle();
	}
}

void
USagaPlayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void
USagaPlayerAnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
}

void
USagaPlayerAnimInstance::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();
}

void
USagaPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}
