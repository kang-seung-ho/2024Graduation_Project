// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../SagaGame/Public/SagaGameInfo.h"
#include "Blueprint/UserWidget.h"
#include "SagaInventoryListWidget.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API USagaInventoryListWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	UFUNCTION(BlueprintCallable)
	void SetItem(UObject* Item);
};
