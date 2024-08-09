// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../SagaGame/Public/SagaGameInfo.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "SagaInventoryListWidget.generated.h"


enum class EEntryWidgetMouseState
{
	None,
	MouseOn,
	Select
};

UCLASS()
class SAGAGAME_API USagaInventoryListWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UImage* mIconImage;
	UImage* mSelection;
	UTextBlock* mCountText;
	class UInventoryItemData* mData = nullptr;
	
protected:
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	void SetMouseState(EEntryWidgetMouseState State);

public:
	UFUNCTION(BlueprintCallable)
	void SetItem(UObject* Item);
};
