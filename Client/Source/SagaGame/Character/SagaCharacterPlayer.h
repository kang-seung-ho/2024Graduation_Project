#pragma once
#include "SagaGame.h"
#include "GameFramework/Character.h"

#include "../SagaGameInfo.h"
#include "Saga/Network/SagaNetworkRpcView.h"
#include "CharacterSelect/SagaSelectCharacter.h"
#include "SagaCharacterPlayer.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaCharacterPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASagaCharacterPlayer();

public:
	int32 id;
	bool isForwardWalking;
	bool isStrafeWalking;
	bool isRunning;
	bool isRiding = false;

	FVector walkDirection;

public:
	int32 teamColor = 0;

	void SetTeamColorAndCollision(int16 Team); //1�̸� ����, 2�̸� ����

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	EPlayerWeapon mWeaponType;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mArm;

	class USagaPlayerAnimInstance* mAnimInst;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);


public:
	virtual void SwordAttack();



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	void PlayAttackAnimation();
	void RotationCameraArm(float Scale);

public:
	UFUNCTION(BlueprintNativeEvent, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void ExecuteRide();
	UFUNCTION(BlueprintNativeEvent, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void TerminateRide();

	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteAttack();
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void TerminateAttack();

	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteRotation(FVector2D RotationInput);
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void TerminateRotation();

	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteJump();
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void TerminateJump();

	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteRun();
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void TerminateRun();

	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteWalk();
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	virtual void TerminateWalk();


};