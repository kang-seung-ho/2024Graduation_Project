#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "SagaGame/Player/SagaPlayerWeaponTypes.h"
#include "CharacterSelect/SagaSelectCharacter.h"
#include "SagaPlayerAnimInstance.generated.h"

UCLASS(Blueprintable, Category = "CandyLandSaga|Game|Character|Animation")
class SAGAGAME_API USagaPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// the below functions are the native overrides for each phase
	// Native initialization override point
	virtual void NativeInitializeAnimation();
	// Native update override point. It is usually a good idea to simply gather data in this step and 
	// for the bulk of the work to be done in NativeThreadSafeUpdateAnimation.
	virtual void NativeUpdateAnimation(float DeltaSeconds);
	// Native thread safe update override point. Executed on a worker thread just prior to graph update 
	// for linked anim instances, only called when the hosting node(s) are relevant
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds);
	// Native Post Evaluate override point
	virtual void NativePostEvaluateAnimation();
	// Native Uninitialize override point
	virtual void NativeUninitializeAnimation();

	// Executed when begin play is called on the owning component
	virtual void NativeBeginPlay();

public:
	void PlayAttackMontage();
	void Death();
	void Hit();
	void Revive();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EPlayerWeapon mWeaponTypes;

public:
	UFUNCTION()
	void AnimNotify_Attack();

	UFUNCTION()
	void AnimNotify_AttackEnable();

	UFUNCTION()
	void AnimNotify_AttackEnd();

	UFUNCTION()
	void AnimNotify_HitRecovery();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float mMoveSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float	mMoveDir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	mJump = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	mIsGround = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	mIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float mHitRecovery = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* mHitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UAnimMontage*> mAttackMontageArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UAnimMontage*> mGunAttackMontageArray;

	int32 mAttackIndex = 0;
	bool mAttackEnable = true;
};
