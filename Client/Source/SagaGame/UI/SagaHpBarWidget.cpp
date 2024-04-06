// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SagaHpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Interface/SagaCharacterWidgetInterface.h"

USagaHpBarWidget::USagaHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxHp = -1.0f;
}

void USagaHpBarWidget::NativeConstruct() //위젯초기화될때 포인터 가져오는 역할
{
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensure(HpProgressBar); //컨트롤이 반드시 있어야 하기에 ensure로 작성하였음

	ISagaCharacterWidgetInterface* CharacterWidget = Cast<ISagaCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}

void USagaHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	ensure(MaxHp > 0.0f);
	if (HpProgressBar)
	{
		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
	}
}
