#pragma once
#include "SagaGame.h"
#include "Player/SagaPlayerTeam.h"
#include "Player/SagaPlayerWeaponTypes.h"
#include "Component/SagaCharacterStatComponent.h"

#include "Saga/Network/SagaVirtualUser.h"
#include "SagaCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnCharacterDeath, class ASagaCharacterBase*, character);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnCharacterRespawned, class ASagaCharacterBase*, character);

UCLASS(BlueprintType, Abstract, NotPlaceable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaCharacterBase : public ACharacter
{
	GENERATED_BODY()

protected:
	static TMap<EPlayerWeapon, TObjectPtr<class UStaticMesh>> WeaponMeshes;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	FSagaVirtualUser ownerData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	TObjectPtr<class USagaCharacterStatComponent> myGameStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	TObjectPtr<class UStaticMeshComponent> myWeapon;

	/* 애니메이션 관련 필드 (애니메이션 인스턴스, 이동, 공격, ...) */
#pragma region =========================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Animation")
	TObjectPtr<class UAnimInstance> myAnimationInst;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Animation")
	TObjectPtr<class USagaPlayerAnimInstance> mAnimInst;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Animation")
	TObjectPtr<class USagaGummyBearAnimInstance> mBearAnimInst;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Animation")
	float animationMoveSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Animation", Meta = (BlueprintGetter = GetMoveAnimationAngle))
	float animationMoveAngle;
#pragma endregion

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	TObjectPtr<class USagaWidgetComponent> myHealthIndicatorBarWidget;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "CandyLandSaga|Game|Character")
	FSagaEventOnCharacterDeath OnCharacterDeath;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "CandyLandSaga|Game|Character")
	FSagaEventOnCharacterRespawned OnCharacterRespawned;

	ASagaCharacterBase();

	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float delta_time) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	virtual void Attack() {};

	UFUNCTION()
	void SetUserId(const int32& id) noexcept;
	/* SetTeamColorAndCollision과 통합 */
	UFUNCTION()
	void SetTeam(const ESagaPlayerTeam& team);
	UFUNCTION()
	void SetWeapon(const EPlayerWeapon& weapon) noexcept;
	UFUNCTION()
	virtual void AttachWeapon();
	UFUNCTION()
	void SetHealth(const float hp) noexcept;
	// 다른 사가 캐릭터에게 속성 전달
	UFUNCTION()
	virtual void TranslateProperties(ASagaCharacterBase* other) const;

	UFUNCTION(BlueprintPure)
	int32 GetUserId() const noexcept;
	UFUNCTION(BlueprintPure)
	ESagaPlayerTeam GetTeam() const noexcept;
	UFUNCTION(BlueprintPure)
	EPlayerWeapon GetWeapon() const noexcept;
	UFUNCTION(BlueprintPure)
	float GetHealth() const noexcept;
	UFUNCTION(BlueprintPure)
	bool HasValidOwnerId() const noexcept;

	/* 액션 메서드 */
#pragma region =========================
	UFUNCTION()
	virtual void ExecuteStraightWalk(const int& direction) noexcept;
	UFUNCTION()
	virtual void TerminateStraightWalk();
	UFUNCTION()
	virtual void ExecuteStrafeWalk(const int& direction) noexcept;
	UFUNCTION()
	virtual void TerminateStrafeWalk();

	UFUNCTION()
	virtual void ExecuteRun();
	UFUNCTION()
	virtual void TerminateRun();

	UFUNCTION()
	virtual void ExecuteJump();
	UFUNCTION()
	virtual void TerminateJump();

	UFUNCTION()
	void ExecuteRotate(const float pitch);

	UFUNCTION()
	virtual void ExecuteAttack();
	UFUNCTION()
	virtual void TerminateAttack();

	/*
	* 수호자 승차
	*/
	UFUNCTION()
	virtual void ExecuteRide();
	/*
	* 수호자 하차
	*/
	UFUNCTION()
	virtual void TerminateRide();

	// TODO: ExecuteHurt에 맞은 무기 종류를 전달하도록 추가
	UFUNCTION()
	virtual float ExecuteHurt(const float dmg);
	UFUNCTION()
	virtual void ExecuteDeath();

	UFUNCTION()
	virtual void ExecuteRespawn();
#pragma endregion

	UFUNCTION(BlueprintPure)
	float GetMoveAnimationSpeed() const noexcept
	{
		return std::min(1.0f, animationMoveSpeed / GetMaxMoveSpeed(true)) * 2;
	}
	UFUNCTION(BlueprintPure)
	float GetMoveAnimationAngle() const noexcept
	{
		return animationMoveAngle;
	}

	UFUNCTION(BlueprintPure)
	bool IsAlive() const noexcept;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	virtual void PlayAttackAnimation();

	UFUNCTION(BlueprintPure)
	virtual float GetMaxMoveSpeed(const bool is_running) const noexcept
	{
		return is_running ? 700 : 400;
	}

	UFUNCTION(BlueprintPure)
	virtual float GetHorizontalMoveAcceleration() const noexcept
	{
		return isRunning ? 100 : 50;
	}

	UFUNCTION(BlueprintPure)
	virtual float GetVerticalMoveAcceleration() const noexcept
	{
		return isRunning ? 300 : 200;
	}

	UFUNCTION(BlueprintPure)
	virtual float GetMoveAnimationDirectionDelta() const noexcept
	{
		return isRunning ? 200 : 100;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	UCameraComponent* myCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	USpringArmComponent* myCameraSpringArmComponent;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	int straightMoveDirection;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	int strafeMoveDirection;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	bool isRunning;
};
