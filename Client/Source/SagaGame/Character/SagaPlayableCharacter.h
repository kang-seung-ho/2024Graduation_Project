#pragma once
#include "SagaGame.h"
#include <Misc/OutputDeviceNull.h>
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>

#include "Character/SagaCharacterBase.h"
#include "SagaPlayableCharacter.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaPlayableCharacter : public ASagaCharacterBase
{
	GENERATED_BODY()
	
public:
	ASagaPlayableCharacter();

	virtual void Attack() override;

	UFUNCTION()
	void BeginRespawn();

	virtual float ExecuteHurt(const float dmg) override;
	virtual void ExecuteDeath() override;

	virtual void ExecuteRespawn() override;

	UFUNCTION()
	void HandleRespawnCountdown();

	UFUNCTION()
	void RideNPC();
	//UFUNCTION(BlueprintImplementableEvent, Category = "Blueprint")
	//void RidingFunction();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSoftClassPtr<class UAnimInstance> humanCharacterAnimation;

	UPROPERTY()
	UParticleSystem* HitCascadeEffect;

	UPROPERTY()
	UParticleSystem* GunHitCascadeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* HitSoundEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* DeadSoundEffect;

	UPROPERTY()
	FTimerHandle respawnTimerHandle;
};
