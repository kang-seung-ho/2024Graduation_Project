#pragma once
#include "SagaGame.h"
#include "GameFramework/Character.h"

#include "../SagaGameInfo.h"
#include "CharacterSelect/SagaSelectCharacter.h"
#include "SagaGame/Player/SagaUserTeam.h"
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
	virtual void Attack();
	UFUNCTION()
	void PlayAttackAnimation();
	UFUNCTION()
	void SetTeamColorAndCollision();
	void SetTeamColorAndCollision(EUserTeam myTeam);
	UFUNCTION()
	virtual void ProcessStraightWalk(const int& direction) noexcept;
	UFUNCTION()
	virtual void ProcessStrafeWalk(const int& direction) noexcept;
	UFUNCTION()
	void RotationCameraArm(float Scale);

	/*
		Speed: 속도 (스칼라)
		Velocity: 속력 (벡터)
		Angle: 각도 - double
		Direction: 방향 - double, FVector
	*/

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
	virtual double GetAnimationDirectionDelta() const noexcept
	{
		return isRunning ? 200 : 100;
	}

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	int32 myId;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character")
	EUserTeam myTEAM;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	EPlayerWeapon mWeaponType;

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
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteWalk();
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	virtual void TerminateStraightWalk();
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	virtual void TerminateStrafeWalk();

	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteRun();
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	virtual void TerminateRun();

	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteJump();
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	virtual void TerminateJump();

	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteAttack();
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	virtual void TerminateAttack();

	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteRide();
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	virtual void TerminateRide();
};
