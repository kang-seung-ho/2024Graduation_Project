// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../SagaGame/Public/SagaGameInfo.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItemData.generated.h"

/**
 * 
 */
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
	bool IsSelect() const
	{
		return mSelect;
	}

	bool IsMouseOn() const
	{
		return mOnMouse;
	}

	void SetSelect(bool Select)
	{
		mSelect = Select;
	}

	void SetMouseOn(bool MouseOn)
	{
		mOnMouse = MouseOn;
	}


	UTexture2D* GetIconTexture()
	{
		return mIconTexture;
	}

	const FString& GetItemName()
	{
		return mName;
	}

	int32 GetCount()
	{
		return mCount;
	}


	void SetInfo(UTexture2D* Icon, const FString& Name, int32 Count)
	{
		mIconTexture = Icon;
		mName = Name;
		mCount = Count;

		UE_LOG(LogTemp, Warning, TEXT("SetInfo called: %s"), *mName);  // SetInfo 호출 로그
	}

	void SetInfo(const FString* IconPath, const FString& Name, int32 Count)
	{
		mIconTexture = LoadObject<UTexture2D>(GetWorld(), **IconPath);
		mName = Name;
		mCount = Count;
	}

	void AddCount(int32 Count)
	{
		mCount += Count;
	}

};
