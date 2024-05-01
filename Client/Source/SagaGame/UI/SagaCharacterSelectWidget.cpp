// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaCharacterSelectWidget.h"

void USagaCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	mStartButton = Cast<UButton>(GetWidgetFromName(TEXT("StartButton")));
	if (mStartButton)
	{
		mStartButton->OnClicked.AddDynamic(this, &USagaCharacterSelectWidget::StartButtonClick);
	}
}

void USagaCharacterSelectWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void USagaCharacterSelectWidget::StartButtonClick()
{
}
