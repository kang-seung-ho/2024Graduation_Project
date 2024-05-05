#include "SagaPlayerAnimInstance.h"
#include "SagaPlayableCharacter.h"
#include "SagaInGamePlayerController.h"

void USagaPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	mMoveSpeed = 0.f;
	mMoveDir = 0.f;
}

void USagaPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ASagaCharacterPlayer* PlayerCharacter = Cast<ASagaCharacterPlayer>(TryGetPawnOwner());

	if (IsValid(PlayerCharacter))
	{
		ASagaPlayableCharacter* WeaponHoldingCharacter = Cast<ASagaPlayableCharacter>(PlayerCharacter);
		if (IsValid(WeaponHoldingCharacter))
		{
			mWeaponTypes = WeaponHoldingCharacter->GetWeaponType();
		}

		UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement();
		if (IsValid(Movement))
		{
			//구해준 이동속도를 최대속도로 나누어 비율구함
			mMoveSpeed = Movement->Velocity.Length();
			mMoveSpeed /= Movement->MaxWalkSpeed;
		}

		ASagaInGamePlayerController* Controller = PlayerCharacter->GetController<ASagaInGamePlayerController>();
		if (IsValid(Controller))
		{
			mMoveDir = Controller->GetMoveDir();
		}

		
	}
}

void USagaPlayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void USagaPlayerAnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
}

void USagaPlayerAnimInstance::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();
}

void USagaPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void USagaPlayerAnimInstance::PlayAttackMontage()
{
	if (!mAttackEnable)
		return;

	mAttackEnable = false;

	if (!Montage_IsPlaying(mAttackMontageArray[mAttackIndex]))
	{
		if (mWeaponTypes == EPlayerWeapon::LightSabor) {
			Montage_SetPosition(mAttackMontageArray[mAttackIndex], 0.f);

			Montage_Play(mAttackMontageArray[mAttackIndex]);

			mAttackIndex = (mAttackIndex + 1) % mAttackMontageArray.Num();
		}
		else if (mWeaponTypes == EPlayerWeapon::WaterGun) {
			Montage_SetPosition(mGunAttackMontageArray[mAttackIndex], 0.f);

			Montage_Play(mGunAttackMontageArray[mAttackIndex]);

			mAttackIndex = (mAttackIndex + 1) % mGunAttackMontageArray.Num();
		}
		
	}
}

void USagaPlayerAnimInstance::AnimNotify_Attack()
{
	ASagaCharacterPlayer* PlayerCharacter = Cast<ASagaCharacterPlayer>(TryGetPawnOwner());
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->SwordAttack();
	}
}

void USagaPlayerAnimInstance::AnimNotify_AttackEnable()
{
	mAttackEnable = true;
}

void USagaPlayerAnimInstance::AnimNotify_AttackEnd()
{	
	mAttackIndex = 0;
}
