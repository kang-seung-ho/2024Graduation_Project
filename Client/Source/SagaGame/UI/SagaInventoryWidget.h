#pragma once
#include "SagaGame.h"
#include <UObject/ObjectPtr.h>
#include <Blueprint/UserWidget.h>

#include "Saga/Interface/SagaUserWidget.h"
#include "Item/SagaItemTypes.h"
#include "SagaInventoryWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|System")
class SAGAGAME_API USagaInventoryWidget : public USagaUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 mEnergyDrinkCount{ 0 };

	UPROPERTY()
	int32 mGumballCount{ 0 };

	UPROPERTY()
	int32 mSmokeBombCount{ 0 };

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* EnergyDrinkAmount;

	UFUNCTION()
	void UpdateEnergyDrinkAmount();

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GumballAmount;

	UFUNCTION()
	void UpdateGumballAmount();

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SmokeBombAmount;

	UFUNCTION()
	void UpdateSmokeBombAmount();



	UPROPERTY()
	TObjectPtr<class UListView> mInventory;
	UPROPERTY()
	TObjectPtr<class UInventoryItemData> mSelectionItem;
	
	UPROPERTY()
	bool awaitingItemRpc{ false };

	UFUNCTION()
	void AddItemToInventory(ESagaItemTypes ItemType);

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
		return mEnergyDrinkCount;
	}

	int32 GetSmokeBombItemCount()
	{
		return mGumballCount;
	}

	int32 GetGumballItemCount()
	{
		return mSmokeBombCount;
	}

protected:
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
};
