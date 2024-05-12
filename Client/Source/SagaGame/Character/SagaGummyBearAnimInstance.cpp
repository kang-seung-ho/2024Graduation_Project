#include "SagaGummyBearAnimInstance.h"
#include "SagaGummyBearPlayer.h"

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

	ASagaGummyBearPlayer* character = Cast<ASagaGummyBearPlayer>(TryGetPawnOwner());

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
