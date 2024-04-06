// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
//#include "Character/SagaCharacterPlayer.h"
#include "Items.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class SAGAGAME_API UItems : public UObject
{
	GENERATED_BODY()
	
public:
	UItems();

	virtual class UWorld* GetWorld() const { return World; };


	UPROPERTY(Transient) //아이템이 어떤 월드에 있었는지
	class UWorld* World;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	FText UseActionText; //아이템 설명 텍스트 (사용, 설치 구분 등)

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	class UStaticMesh* PickupMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	class UTexture2D* Thumbnail;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item)
	FText ItemDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item, meta = (MultiLine = true)) //(MultiLine = true)로 인해 새 줄 추가 가능
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Item, meta = (ClampMin = 0.0)) //인벤토리 무게제한(아이템별 무게) 사용할거면 냅두기
	float Weight;

	UPROPERTY()
	class UInventoryComponent* OwningInventory;

	virtual void Use(class ASagaCharacterPlayer* Character);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUse(class ASagaCharacterPlayer* Character);
};
