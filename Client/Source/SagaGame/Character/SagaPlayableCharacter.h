#pragma once
#include "SagaGame.h"

#include "Character/SagaCharacterBase.h"
#include "SagaPlayableCharacter.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaPlayableCharacter : public ASagaCharacterBase
{
	GENERATED_BODY()
	
public:
	ASagaPlayableCharacter();

	UFUNCTION()
	void RideNPC();
	//UFUNCTION(BlueprintImplementableEvent, Category = "Blueprint")
	//void RidingFunction();

	UFUNCTION()
	void BeginRespawn();

	// UFUNCTION()
	virtual void ExecuteGuardianAction(ASagaCharacterBase* target) override;
	// UFUNCTION()
	virtual void TerminateGuardianAction() override;
	// UFUNCTION()
	virtual void ExecuteAttackAnimation() override;
	// UFUNCTION()
	virtual void ExecuteAttack() override;
	// UFUNCTION()
	virtual float ExecuteHurt(const float dmg) override;
	// UFUNCTION()
	virtual void ExecuteDeath() override;
	// UFUNCTION()
	virtual void ExecuteRespawn() override;

	UFUNCTION()
	class ASagaGummyBearPlayer* GetNeareastCollidedBear() const;
	UFUNCTION()
	bool HasCollidedBear() const noexcept;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Animation")
	TSoftClassPtr<class UAnimInstance> humanCharacterAnimation;

	UPROPERTY()
	TArray<TObjectPtr<class ASagaGummyBearPlayer>> collideBears;

	UPROPERTY()
	class UParticleSystem* HitCascadeEffect;
	UPROPERTY()
	class UParticleSystem* GunHitCascadeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundBase* HitSoundEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundBase* DeadSoundEffect;

	UPROPERTY()
	FTimerHandle respawnTimerHandle;

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleBeginCollision(AActor* self, AActor* other_actor);
	UFUNCTION()
	void HandleEndCollision(AActor* self, AActor* other_actor);
	UFUNCTION()
	void HandleRespawnCountdown();
};
