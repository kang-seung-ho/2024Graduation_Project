#pragma once
#include "SagaGame.h"
#include "GameFramework/Character.h"

#include "SagaGameInfo.h"
#include "Player/SagaUserTeam.h"
#include "Player/SagaPlayerWeaponTypes.h"
#include "SagaCharacterPlayer.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaCharacterPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASagaCharacterPlayer();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float delta_time) override;
	// Called to bind functionality to input
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void SetUserId(const int32& id) noexcept;
	UFUNCTION()
	void SetTeamColorAndCollision(const EUserTeam& myTeam) noexcept;
	UFUNCTION()
	void SetWeapon(const EPlayerWeapon& weapon) noexcept;
	UFUNCTION(BlueprintPure)
	int32 GetUserId() const noexcept;
	UFUNCTION(BlueprintPure)
	EUserTeam GetTeamColorAndCollision() const noexcept;
	UFUNCTION(BlueprintPure)
	EPlayerWeapon GetWeapon() const noexcept;

	UFUNCTION()
	virtual void Attack();
	UFUNCTION()
	void PlayAttackAnimation();
	UFUNCTION()
	virtual void TranslateProperties(ASagaCharacterPlayer* other) const;

	UFUNCTION()
	void RotateCameraArm(const float pitch);

	/*
		Speed: 속도 (스칼라)
		Velocity: 속력 (벡터)
		Angle: 각도 - double
		Direction: 방향 - double, FVector
	*/

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

	friend class ASagaInGamePlayerController;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	UFUNCTION()
	virtual void ProcessMovement();
	UFUNCTION()
	virtual void ProcessAnimation(const float& delta_time);

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
	int32 myId;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	EUserTeam myTeam;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	EPlayerWeapon myWeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	float animationMoveSpeed; // 애니메이션 전용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (BlueprintGetter = GetMoveAnimationAngle))
	float animationMoveAngle; // 애니메이션 전용
	class USagaPlayerAnimInstance* mAnimInst;
	class USagaGummyBearAnimInstance* mBearAnimInst;

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	UCameraComponent* mCamera;
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	USpringArmComponent* mArm;

protected:
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
};
