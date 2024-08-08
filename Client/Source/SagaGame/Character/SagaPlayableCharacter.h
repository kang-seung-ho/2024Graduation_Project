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

	/* 데미지 처리

	* 온라인 / 오프라인 모드에 따라 분기
	*  온라인 모드	: RPC만 보냄. 나중에 RPC 돌려받았을 때 ASagaCharacterBase의 TakeDamage를 실행.
	*  오프라인 모드	: ASagaCharacterBase의 TakeDamage를 그대로 실행.
	*/
	virtual float TakeDamage(float dmg, struct FDamageEvent const& event, AController* instigator, AActor* causer) override;

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
	/* 리스폰 RPC는 기본적으로 서버에서 바로 브로드캐스트해줌 */
	// UFUNCTION()
	virtual void ExecuteRespawn() override;
	UFUNCTION()
	void ExecuteRespawnViaRpc();

	UFUNCTION()
	class ASagaGummyBearPlayer* GetNeareastCollidedBear() const;

	virtual void UseSkill(int32 SlotNumber);
	void ExecuteSkill(int32 SlotNumber);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Animation")
	TSoftClassPtr<class UAnimInstance> humanCharacterAnimation;

	UPROPERTY()
	TArray<class ASagaGummyBearPlayer*> collideBears;

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
