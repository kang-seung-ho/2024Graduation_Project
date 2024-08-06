#include "SagaGummyBearAnimInstance.h"
#include <HAL/Platform.h>
#include <Templates/Casts.h>
#include <Logging/LogMacros.h>
#include <UObject/Object.h>

#include "SagaGummyBearPlayer.h"

void
USagaGummyBearAnimInstance::PlayAttackMontage()
{
	if (!mAttackEnable)
		return;

	mAttackEnable = false;

	UE_LOG(LogSagaGame, Warning, TEXT("PlayAttackMontage"));

	if (!Montage_IsPlaying(mAttackMontageArray[mAttackIndex]))
	{
		Montage_SetPosition(mAttackMontageArray[mAttackIndex], 0.f);

		Montage_Play(mAttackMontageArray[mAttackIndex]);

		mAttackIndex = (mAttackIndex + 1) % mAttackMontageArray.Num();
	}
}

void USagaGummyBearAnimInstance::Death()
{
	mIsDead = true;
}


void
USagaGummyBearAnimInstance::AnimNotify_Attack()
{
	const auto character = GetPawnOwner();

	if (IsValid(character))
	{
		character->ExecuteAttack();
	}
}

void
USagaGummyBearAnimInstance::AnimNotify_AttackEnable()
{
	mAttackEnable = true;
}

void
USagaGummyBearAnimInstance::AnimNotify_AttackEnd()
{
	mAttackIndex = 0;
}

ASagaGummyBearPlayer*
USagaGummyBearAnimInstance::GetPawnOwner()
const
{
	return Cast<ASagaGummyBearPlayer>(TryGetPawnOwner());
}

void
USagaGummyBearAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	mMoveSpeed = 0.f;
	mMoveDir = 0.f;
}

void
USagaGummyBearAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const auto character = GetPawnOwner();

	if (IsValid(character))
	{
		mMoveSpeed = character->GetMoveAnimationSpeed();
		mMoveDir = character->GetMoveAnimationAngle();
	}
}

void
USagaGummyBearAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void
USagaGummyBearAnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
}

void
USagaGummyBearAnimInstance::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();
}

void
USagaGummyBearAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}
