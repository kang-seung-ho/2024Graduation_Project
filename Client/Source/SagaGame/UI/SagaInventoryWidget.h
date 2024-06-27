#pragma once
#include "SagaGame.h"
#include "Saga/Interface/SagaUserWidget.h"
#include <UObject/ObjectPtr.h>
#include <Blueprint/UserWidget.h>

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

	void AddItemToInventory(class UInventoryItemData* ItemData);
};
