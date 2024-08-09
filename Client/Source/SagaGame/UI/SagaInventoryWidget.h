#pragma once
#include "SagaGame.h"
#include <UObject/ObjectPtr.h>
#include <Blueprint/UserWidget.h>

#include "Saga/Interface/SagaUserWidget.h"
#include "SagaInventoryWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|System")
class SAGAGAME_API USagaInventoryWidget : public USagaUserWidget
{
	GENERATED_BODY()

public:
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
	void UseEnergyDrink();
	UFUNCTION()
	void UseGumball();
	UFUNCTION()
	void UseSmokeBomb();

	int32 GetEnergyDrinkItemCount()
	{
		return 0;
	}

	int32 GetSmokeBombItemCount()
	{
		return 0;
	}

	int32 GetGumballItemCount()
	{
		return 0;
	}

protected:
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
};
