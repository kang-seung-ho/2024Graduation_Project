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

	virtual void Attack();
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void PlayAttackAnimation();
	UFUNCTION()
	virtual void ProcessForwardWalk(const int& direction) noexcept;
	UFUNCTION()
	virtual void ProcessStrafeWalk(const int& direction) noexcept;
	UFUNCTION()
	void RotationCameraArm(float Scale);
	UFUNCTION()
	void SetTeamColorAndCollision();


	void SetTeamColorAndCollision(EUserTeam myTeam);

	virtual void Tick(float delta_time) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
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

	UFUNCTION(BlueprintNativeEvent, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void ExecuteJump();
	UFUNCTION(BlueprintNativeEvent, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void TerminateJump();

	UFUNCTION(BlueprintNativeEvent, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void ExecuteAttack();
	UFUNCTION(BlueprintNativeEvent, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void TerminateAttack();

	UFUNCTION(BlueprintNativeEvent, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void ExecuteRide();
	UFUNCTION(BlueprintNativeEvent, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void TerminateRide();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	int32 myId;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	FVector walkDirection;

protected:
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	FVector preferedDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	bool isForwardWalking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	bool isStrafeWalking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	bool isRunning;
	UPROPERTY()
	bool wasMoved; // 이동했는지 여부
	UPROPERTY()
	bool wasTilted; // 회전했는지 여부

	UPROPERTY(VisibleAnywhere, AdvancedDisplay, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	FTimerHandle tranformUpdateTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character")
	EUserTeam myTEAM;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	EPlayerWeapon mWeaponType;

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	UCameraComponent* mCamera;

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	USpringArmComponent* mArm;

	class USagaPlayerAnimInstance* mAnimInst;
	class USagaGummyBearAnimInstance* mBearAnimInst;
};
