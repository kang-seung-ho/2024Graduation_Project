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

public:
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
