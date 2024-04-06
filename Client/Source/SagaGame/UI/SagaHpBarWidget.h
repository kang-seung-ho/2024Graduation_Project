#pragma once
#include "CoreMinimal.h"
#include "Saga/Interface/SagaUserWidget.h"

#include "SagaHpBarWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Interface")
class SAGAGAME_API USagaHpBarWidget : public USagaUserWidget
{
	GENERATED_BODY()
	
public:
	USagaHpBarWidget(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }
	void UpdateHpBar(float NewCurrentHp);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
	float MaxHp;
};
