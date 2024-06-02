#pragma once
#include "SagaGame.h"
#include <Blueprint/UserWidget.h>

#include "Saga/Interface/SagaUserWidget.h"
#include "SagaHpBarWidget.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|UI")
class SAGAGAME_API USagaHpBarWidget : public USagaUserWidget
{
	GENERATED_BODY()

public:
	USagaHpBarWidget(const FObjectInitializer& initializer) noexcept;

	UFUNCTION()
	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }
	UFUNCTION()
	void UpdateHpBar(float CurrentHp);
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
	float MaxHp;
};
