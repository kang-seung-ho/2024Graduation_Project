#pragma once
#include "SagaGame.h"
#include "Misc/OutputDeviceNull.h"
#include "Character/SagaCharacterBase.h"

#include "../Item/SagaWeaponData.h"
#include "../Interface/SagaCharacterItemInterface.h"
#include "Player/SagaPlayerWeaponTypes.h"
#include "CharacterSelect/SagaSelectCharacter.h"
#include "SagaGummyBearPlayer.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "SagaPlayableCharacter.generated.h"


UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaPlayableCharacter : public ASagaCharacterBase
{
	GENERATED_BODY()
	
public:
	ASagaPlayableCharacter();

	virtual void Attack() override;

	virtual float ExecuteHurt(const float dmg) override;
	virtual void ExecuteDeath() override;
	UFUNCTION()
	void BeginRespawn();
	virtual void ExecuteRespawn() override;

	UFUNCTION()
	void HandleRespawnCountdown();

	UFUNCTION()
	void RideNPC();
	//UFUNCTION(BlueprintImplementableEvent, Category = "Blueprint")
	//void RidingFunction();

	UFUNCTION()
	EPlayerWeapon GetWeaponType() const noexcept { return myWeaponType; }

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	UPROPERTY()
	FTimerHandle RespawnTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UAnimInstance> humanCharacterAnimation;

	UPROPERTY()
	UParticleSystem* HitCascadeEffect;

	UPROPERTY()
	UParticleSystem* GunHitCascadeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* HitSoundEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* DeadSoundEffect;
};
