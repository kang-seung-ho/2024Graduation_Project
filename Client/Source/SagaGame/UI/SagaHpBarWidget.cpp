// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SagaHpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Interface/SagaCharacterWidgetInterface.h"

USagaHpBarWidget::USagaHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxHp = -1.0f;
}

void USagaHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PlayerHpBar")));
	ensure(HpProgressBar);

	ISagaCharacterWidgetInterface* CharacterWidget = Cast<ISagaCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}

void USagaHpBarWidget::UpdateHpBar(float CurrentHp)
{
	ensure(MaxHp > 0.0f);
	if (HpProgressBar)
	{
		float NewPercent = CurrentHp / MaxHp;
		HpProgressBar->SetPercent(NewPercent);
		UE_LOG(LogTemp, Log, TEXT("CurrentHp: %f, MaxHp: %f, Percent: %f"), CurrentHp, MaxHp, NewPercent);
	}
}
