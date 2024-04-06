#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SagaCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*CurrentHp*/); //변경된 현재 hp값을 구독한 객체에 보내도록

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SAGAGAME_API USagaCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USagaCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;

	FORCEINLINE float GetMaxHp() { return MaxHp; }
	FORCEINLINE float GetCurrentHp() { return CurrentHp; }
	float ApplyDamage(float InDamage);

protected:
	void SetHp(float NewHp);

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float MaxHp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat) //Transient: 컴 디스크에 저장 안함(실시간으로 변화하는데이터)
	float CurrentHp;
		
};
