#pragma once
#include "SagaGameInfo.h"

#include "InventoryItemData.generated.h"

UCLASS()
class SAGAGAME_API UInventoryItemData : public UObject
{
	GENERATED_BODY()
	
private:
	UTexture2D* mIconTexture;
	FString mName;
	int32 mCount;
	bool mSelect = false;
	bool mOnMouse = false;

public:
	bool IsMouseOn() const
	{
		return mOnMouse;
	}

	UFUNCTION()
	void SetSelect(bool Select)
	{
		mSelect = Select;
	}

	UFUNCTION()
	void SetInfo(UTexture2D* Icon, const FString& Name, int32 Count)
	{
		mIconTexture = Icon;
		mName = Name;
		mCount = Count;

		UE_LOG(LogTemp, Warning, TEXT("SetInfo called: %s"), *mName);
	}

	UFUNCTION()
	void SetInfoFrom(const FString& IconPath, const FString& Name, int32 Count)
	{
		mIconTexture = LoadObject<UTexture2D>(GetWorld(), *IconPath);
		mName = Name;
		mCount = Count;
	}

	UFUNCTION()
	void AddCount(int32 Count)
	{
		mCount += Count;
	}

	UFUNCTION()
	void SetMouseOn(bool MouseOn)
	{
		mOnMouse = MouseOn;
	}

	UFUNCTION()
	UTexture2D* GetIconTexture() const noexcept
	{
		return mIconTexture;
	}

	UFUNCTION()
	const FString& GetItemName() const noexcept
	{
		return mName;
	}

	UFUNCTION()
	int32 GetCount() const noexcept
	{
		return mCount;
	}

	UFUNCTION()
	bool IsSelect() const noexcept
	{
		return mSelect;
	}
};
