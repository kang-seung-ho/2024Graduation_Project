// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

//블루프린트가 UI를 업뎃시키기 위해 바인드할것임.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SAGAGAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void BeginPlay() override;

	bool AddItem(class UItems* Item);
	bool RemoveItem(class UItems* Item);

	UPROPERTY(EditDefaultsOnly, Instanced)
	TArray<class UItems*> DefaultItems;

	UPROPERTY(EditDefaultsOnly, Category = Inventory) //보유 아이템 한계치
	int32 Capacity;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Item) //현재 가지고 있는 아이템들
	TArray<class UItems*> Items;

		
};
