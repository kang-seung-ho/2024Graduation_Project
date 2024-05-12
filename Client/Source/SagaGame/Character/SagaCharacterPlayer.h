#pragma once
#include "SagaGame.h"
#include "GameFramework/Character.h"

#include "SagaGameInfo.h"
#include "Player/SagaUserTeam.h"
#include "Player/SagaPlayerWeaponTypes.h"
#include "Component/SagaCharacterStatComponent.h"
#include "Interface/SagaCharacterWidgetInterface.h"
#include "SagaCharacterPlayer.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaCharacterPlayer : public ACharacter, public ISagaCharacterWidgetInterface
{
	GENERATED_BODY()

public:
	/* 게임 속성 메서드 */
#pragma region =========================
	void SetUserId(const int32& id) noexcept;
	UFUNCTION()
	void SetTeamColorAndCollision(const EUserTeam& myTeam) noexcept;
	UFUNCTION()
	void SetWeapon(const EPlayerWeapon& weapon) noexcept;
	UFUNCTION()
	void SetHealth(const float hp) noexcept;
	UFUNCTION(BlueprintPure)
	int32 GetUserId() const noexcept;
	UFUNCTION(BlueprintPure)
	EUserTeam GetTeamColorAndCollision() const noexcept;
	UFUNCTION(BlueprintPure)
	EPlayerWeapon GetWeapon() const noexcept;
	UFUNCTION(BlueprintPure)
	float GetHealth() const noexcept;

	// 다른 사가 캐릭터에게 속성 전달
	UFUNCTION()
	virtual void TranslateProperties(ASagaCharacterPlayer* other) const;
#pragma endregion

	UFUNCTION()
	virtual void Attack();
	UFUNCTION()
	virtual void PlayAttackAnimation();
	UFUNCTION()
	void AttachWeapon(EPlayerWeapon WeaponType);
	UFUNCTION()
	virtual void SetDead();
	UFUNCTION()
	virtual void RespawnCharacter();

	// Called to bind functionality to input
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/*
		Speed: 속도 (스칼라)
		Velocity: 속력 (벡터)
		Angle: 각도 - double
		Direction: 방향 - double, FVector
	*/

	UFUNCTION()
	void RotateCameraArm(const float pitch);

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
	virtual void ExecuteAttack();
	UFUNCTION()
	virtual void TerminateAttack();

	UFUNCTION()
	virtual void ExecuteRide();
	UFUNCTION()
	virtual void TerminateRide();
	
	UFUNCTION()
	virtual float ExecuteHurt(const float dmg);
	UFUNCTION()
	virtual void ExecuteDeath();
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

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character")
	int straightMoveDirection;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character")
	int strafeMoveDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	bool isRunning;

public:
	ASagaCharacterPlayer();

	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float delta_time) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/* 실시간 처리 메서드 (Tick) */
#pragma region =========================
	UFUNCTION()
	virtual void ProcessMovement();
	UFUNCTION()
	virtual void ProcessAnimation(const float& delta_time);
#pragma endregion

	/* 이동 속성 메서드 */
#pragma region =========================
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
#pragma endregion

	UFUNCTION(BlueprintPure)
	virtual float GetMoveAnimationDirectionDelta() const noexcept
	{
		return isRunning ? 200 : 100;
	}

	/* 게임 속성 필드 (ID, 팀, 무기) */
#pragma region =========================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	int32 myId;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	EUserTeam myTeam;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	EPlayerWeapon myWeaponType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	float myHealth;
#pragma endregion

	/* 애니메이션 관련 필드 (애니메이션 인스턴스, 이동, 공격, ...) */
#pragma region =========================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	float animationMoveSpeed; // 애니메이션 전용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (BlueprintGetter = GetMoveAnimationAngle))
	float animationMoveAngle; // 애니메이션 전용
	class USagaPlayerAnimInstance* mAnimInst;
	class USagaGummyBearAnimInstance* mBearAnimInst;
#pragma endregion

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	UCameraComponent* mCamera;
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	USpringArmComponent* mArm;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterStat", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USagaCharacterStatComponent> Stat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USagaWidgetComponent> HpBar;

	virtual void SetupCharacterWidget(class USagaUserWidget* InUserWidget) override;

protected:
	// For saving Weapon Meshes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	TObjectPtr<class UStaticMeshComponent> MyWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TMap<EPlayerWeapon, UStaticMesh*> WeaponMeshes;
};
