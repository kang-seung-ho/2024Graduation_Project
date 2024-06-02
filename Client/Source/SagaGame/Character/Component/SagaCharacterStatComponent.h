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
};

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character", ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SAGAGAME_API USagaCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USagaCharacterStatComponent(const FObjectInitializer& initializer) noexcept;

public:
	static constexpr float defaultMaxHealth = 100;

	UPROPERTY(VisibleInstanceOnly, Category = "CandyLandSaga|Game|Character")
	float MaxHp;
	UPROPERTY(Transient, VisibleInstanceOnly, Category = "CandyLandSaga|Game|Character")
	float CurrentHp;

	UPROPERTY(BlueprintAssignable)
	FSagaEventOnHpZero OnHpZero;
	UPROPERTY(BlueprintAssignable)
	FSagaEventOnHpChanged OnHpChanged;

	UFUNCTION()
	void SetMaxHp(float hp, ESagaMaxHealthUpdatePolicy current_health_policy);
	UFUNCTION()
	void SetHp(float NewHp);
	UFUNCTION()
	float ApplyDamage(float Damage);

	UFUNCTION()
	FORCEINLINE float GetMaxHp() const { return MaxHp; }
	UFUNCTION()
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }

protected:
	virtual void BeginPlay() override;



};
