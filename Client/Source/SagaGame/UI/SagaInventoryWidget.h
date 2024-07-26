#pragma once
#include "SagaGame.h"
#include <UObject/ObjectPtr.h>
#include <Blueprint/UserWidget.h>

#include "Saga/Interface/SagaUserWidget.h"
#include "SagaInventoryWidget.generated.h"

UCLASS()
class SAGAGAME_API USagaInventoryWidget : public USagaUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<class UButton> mCloseButton;
	UPROPERTY()
	TObjectPtr<class UListView> mInventory;
	UPROPERTY()
	TObjectPtr<class UInventoryItemData> mSelectionItem;
	
	UPROPERTY()
	bool awaitingItemRpc{ false };

	UFUNCTION()
	void AddItemToInventory(class UInventoryItemData* ItemData);

	UFUNCTION()
	void OnListItemHover(UObject* Item, bool IsHovered);
	UFUNCTION()
	void OnListItemSelection(UObject* Item);
	UFUNCTION()
	void OnListItemClick(UObject* Item);

	UFUNCTION()
	void UseEnergyDrink(class UInventoryItemData* item_entry);
	UFUNCTION()
	void UseGumball(class UInventoryItemData* item_entry);
	UFUNCTION()
	void UseSmokeBomb(class UInventoryItemData* item_entry);

	UFUNCTION()
	void CloseButtonClick();

protected:
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
};
