// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaGummyBearAnimInstance.h"
#include "SagaGummyBearPlayer.h"
#include "SagaInGamePlayerController.h"

void USagaGummyBearAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	mMoveSpeed = 0.f;
	mMoveDir = 0.f;
}

void USagaGummyBearAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ASagaGummyBearPlayer* BearCharacter = Cast<ASagaGummyBearPlayer>(TryGetPawnOwner());

	if (IsValid(BearCharacter))
	{
		UCharacterMovementComponent* Movement = BearCharacter->GetCharacterMovement();
		if (IsValid(Movement))
		{
			//구해준 이동속도를 최대속도로 나누어 비율구함
			mMoveSpeed = Movement->Velocity.Length();
			mMoveSpeed /= Movement->MaxWalkSpeed;
		}

		ASagaInGamePlayerController* Controller = BearCharacter->GetController<ASagaInGamePlayerController>();
		if (IsValid(Controller))
		{
			mMoveDir = Controller->GetMoveDir();
		}


	}
}

void USagaGummyBearAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void USagaGummyBearAnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
}

void USagaGummyBearAnimInstance::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();
}

void USagaGummyBearAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void USagaGummyBearAnimInstance::PlayAttackMontage()
{
	if (!mAttackEnable)
		return;

	mAttackEnable = false;

	if (!Montage_IsPlaying(mAttackMontageArray[mAttackIndex]))
	{
		Montage_SetPosition(mAttackMontageArray[mAttackIndex], 0.f);

		Montage_Play(mAttackMontageArray[mAttackIndex]);

		mAttackIndex = (mAttackIndex + 1) % mAttackMontageArray.Num();
	}
}

void USagaGummyBearAnimInstance::AnimNotify_Attack()
{

}

void USagaGummyBearAnimInstance::AnimNotify_AttackEnable()
{
	mAttackEnable = true;
}

void USagaGummyBearAnimInstance::AnimNotify_AttackEnd()
{
	mAttackIndex = 0;
}
