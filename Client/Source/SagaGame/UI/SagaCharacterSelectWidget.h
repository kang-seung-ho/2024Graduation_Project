// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../SagaGameInfo.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "SagaCharacterSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API USagaCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UButton* mStartButton;

protected:
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	UFUNCTION()
	void StartButtonClick();

	void StartButtonEnable(bool bEnable);
	
};
