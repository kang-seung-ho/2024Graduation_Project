#pragma once
#include "SagaGame.h"
#include <Components/ActorComponent.h>

#include "SagaCharacterStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSagaEventOnHpZero);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnHpChanged, float, hp);

UENUM()
enum class ESagaMaxHealthUpdatePolicy
{
	Ignore = 0,

	SetCurrentHealthFromRatio,
	SetCurrentHealthToMaxHealth,

	Default = SetCurrentHealthToMaxHealth
};

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character", ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SAGAGAME_API USagaCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static constexpr float defaultMaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character")
	float MaxHp;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Transient, Category = "CandyLandSaga|Game|Character")
	float CurrentHp;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Game|Character")
	FSagaEventOnHpZero OnHpZero;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Game|Character")
	FSagaEventOnHpChanged OnHpChanged;

	USagaCharacterStatComponent(const FObjectInitializer& initializer) noexcept;

	UFUNCTION()
	void SetMaxHp(float max_hp, ESagaMaxHealthUpdatePolicy current_health_policy = ESagaMaxHealthUpdatePolicy::Default);
	UFUNCTION()
	void SetCurrentHp(float hp);
	UFUNCTION()
	float ApplyDamage(float dmg);
	UFUNCTION()
	void ResetHp(bool execute_event = false);
	UFUNCTION()
	void RetryUpdateHealth() const;

	UFUNCTION()
	FORCEINLINE float GetMaxHp() const noexcept { return MaxHp; }
	UFUNCTION()
	FORCEINLINE float GetCurrentHp() const noexcept { return CurrentHp; }

protected:
	virtual void BeginPlay() override;



};
