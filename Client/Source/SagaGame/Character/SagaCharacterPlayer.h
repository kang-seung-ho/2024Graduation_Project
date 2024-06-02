#pragma once
#include "SagaGame.h"
#include <GameFramework/Character.h>
#include <GameFramework/SpringArmComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Camera/CameraComponent.h>

#include "SagaGameInfo.h"
#include "Player/SagaUserTeam.h"
#include "Player/SagaPlayerWeaponTypes.h"
#include "Component/SagaCharacterStatComponent.h"
#include "Interface/SagaCharacterWidgetInterface.h"
#include "SagaCharacterPlayer.generated.h"

UCLASS(BlueprintType, Abstract, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaCharacterPlayer : public ACharacter, public ISagaCharacterWidgetInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	int32 myId;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	EUserTeam myTeam;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	EPlayerWeapon myWeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	float myHealth;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character")
	int straightMoveDirection;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character")
	int strafeMoveDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	bool isRunning;

	/* 애니메이션 관련 필드 (애니메이션 인스턴스, 이동, 공격, ...) */
#pragma region =========================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Animation")
	class USagaPlayerAnimInstance* mAnimInst;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Animation")
	class USagaGummyBearAnimInstance* mBearAnimInst;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Animation")
	float animationMoveSpeed; // 애니메이션 전용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Animation", Meta = (BlueprintGetter = GetMoveAnimationAngle))
	float animationMoveAngle; // 애니메이션 전용
#pragma endregion

	ASagaCharacterPlayer();

	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float delta_time) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void SetUserId(const int32& id) noexcept;
	UFUNCTION()
	void SetTeamColorAndCollision(const EUserTeam& team) noexcept;
	UFUNCTION()
	void SetWeapon(const EPlayerWeapon& weapon) noexcept;
	UFUNCTION()
	virtual void AttachWeapon();
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
	UFUNCTION(BlueprintPure)
	bool HasValidOwnerId() const noexcept;

	// 다른 사가 캐릭터에게 속성 전달
	UFUNCTION()
	virtual void TranslateProperties(ASagaCharacterPlayer* other) const;

	UFUNCTION()
	virtual void Attack() {};

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
	bool IsAlive() const noexcept
	{
		return 0 < myHealth;
	}

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	virtual void PlayAttackAnimation();
	UFUNCTION()
	virtual void SetDead();
	// UFUNCTION
	virtual void SetupCharacterWidget(class USagaUserWidget* InUserWidget) override;

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

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	UCameraComponent* mCamera;
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	USpringArmComponent* mArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USagaCharacterStatComponent> Stat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USagaWidgetComponent> HpBar;

protected:
	// For saving Weapon Meshes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	TObjectPtr<class UStaticMeshComponent> MyWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TMap<EPlayerWeapon, UStaticMesh*> WeaponMeshes;
};
