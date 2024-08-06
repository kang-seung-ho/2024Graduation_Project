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
	UPROPERTY()
	TObjectPtr<class UProgressBar> myProgressBar;

	USagaHpBarWidget(const FObjectInitializer& initializer) noexcept;

	UFUNCTION()
	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }

	UFUNCTION()
	void UpdateHpBar(float CurrentHp);
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UPROPERTY()
	float MaxHp;
};
