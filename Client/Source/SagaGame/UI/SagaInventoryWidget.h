// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../SagaGame/Public/SagaGameInfo.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Blueprint/UserWidget.h"
#include "SagaInventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API USagaInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UButton* mCloseButton;
	UListView* mInventory;
	UInventoryItemData* mSelectionItem;
	
protected:
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	UFUNCTION()
	void CloseButtonClick();

	UFUNCTION()
	void OnListItemHover(UObject* Item, bool IsHovered);

	UFUNCTION()
	void OnListItemSelection(UObject* Item);

	UFUNCTION()
	void OnListItemClick(UObject* Item);

	void UseEnergyDrink();
	void UseGumball();
	void UseSmokeBomb();
};
