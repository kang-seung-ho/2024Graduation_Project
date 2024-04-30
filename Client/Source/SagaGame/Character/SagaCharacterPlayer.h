#pragma once
#include "SagaGame.h"
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

public:
	int16 teamColor = 0;

	void SetTeamColorAndCollision(int16 Team); //1이면 레드, 2이면 블루

protected:
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



};

UCLASS()
class ASagaRemotePlayerCharacter : public ASagaCharacterPlayer, public ISagaNetworkRpcView
{
	GENERATED_BODY()

public:
};
