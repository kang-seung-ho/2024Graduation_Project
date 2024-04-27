#pragma once
#include "GameFramework/Character.h"
#include "../SagaGameInfo.h"
#include "Saga/Network/SagaNetworkRpcView.h"
#include "SagaCharacterPlayer.generated.h"

UCLASS()
class SAGAGAME_API ASagaCharacterPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASagaCharacterPlayer();

protected:
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mArm;

	class USagaPlayerAnimInstance* mAnimInst;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	void PlayAttackAnimation();



};

UCLASS()
class ASagaRemotePlayerCharacter : public ASagaCharacterPlayer, public ISagaNetworkRpcView
{
	GENERATED_BODY()

public:
};
