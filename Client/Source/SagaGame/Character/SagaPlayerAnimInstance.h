#pragma once
#include "SagaGame.h"
#include <Animation/AnimInstance.h>

#include "SagaGame/Player/SagaPlayerWeaponTypes.h"
#include "SagaPlayerAnimInstance.generated.h"

USTRUCT(BlueprintType)
struct FSkillAnim
{
	GENERATED_USTRUCT_BODY()

public:
	FSkillAnim()
		: SlotNumber(0), Anim(nullptr)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SlotNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Anim;
};

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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EPlayerWeapon mWeaponTypes;

	UFUNCTION()
	void PlayAttackMontage();

	UFUNCTION()
	void Death();

	UFUNCTION()
	void Hit();

	UFUNCTION()
	void Revive();

	UFUNCTION()
	void UseSkill(int32 SlotNumber);

	UFUNCTION()
	void AnimNotify_Attack();

	UFUNCTION()
	void AnimNotify_AttackEnable();

	UFUNCTION()
	void AnimNotify_AttackEnd();

	UFUNCTION()
	void AnimNotify_HitRecovery();

	UFUNCTION()
	void AnimNotify_Skill_Q();

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UAnimMontage*> mHammerAttackMontageArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSkillAnim> mSkillAnimArray;

	UPROPERTY()
	int32 mAttackIndex = 0;

	UPROPERTY()
	bool mAttackEnable = true;

	UFUNCTION()
	class ASagaPlayableCharacter* GetPawnOwner() const;
};
