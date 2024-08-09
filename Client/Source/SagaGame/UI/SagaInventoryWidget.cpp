#include "UI/SagaInventoryWidget.h"
#include <Components/Button.h>
#include <Components/ListView.h>
#include <Blueprint/UserWidget.h>

#include "InventoryItemData.h"
#include "SagaInventoryListWidget.h"
#include "Character/SagaCharacterBase.h"
#include "Item/SagaItemTypes.h"
#include "PlayerControllers/SagaInGamePlayerController.h"
#include "UI/InventoryItemData.h"

#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaVirtualUser.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

//#include "Engine/World.h"

void
USagaInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();



	mInventory = Cast<UListView>(GetWidgetFromName(TEXT("InventoryList")));
	if (mInventory)
	{
		mInventory->OnItemIsHoveredChanged().AddUObject(this, &USagaInventoryWidget::OnListItemHover);
		mInventory->OnItemSelectionChanged().AddUObject(this, &USagaInventoryWidget::OnListItemSelection);
		mInventory->OnItemClicked().AddUObject(this, &USagaInventoryWidget::OnListItemClick);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryList not found!"));
	}

	FString IconPath[3] =
	{
		TEXT("/Script/Engine.Texture2D'/Game/UI/Images/Tex_heart.Tex_heart'"),
		TEXT("/Script/Engine.Texture2D'/Game/UI/Images/gumball_machine.gumball_machine'"),
		TEXT("/Script/Engine.Texture2D'/Game/UI/Images/smoke_bomb.smoke_bomb'")
	};

	FString ItemName[3] =
	{
		TEXT("EnergyDrink"),
		TEXT("Gumball"),
		TEXT("SmokeBomb")
	};

	//for (int32 i = 0; i < 10; ++i)
	//{
	//	UInventoryItemData* ItemData = NewObject<UInventoryItemData>();

	//	int32 IconIndex = FMath::RandRange(0, 2);
	//	UTexture2D* IconTexture = LoadObject<UTexture2D>(nullptr, *IconPath[IconIndex]);

	//	ItemData->SetInfo(IconTexture, ItemName[IconIndex], 1);

	//	UE_LOG(LogTemp, Warning, TEXT("Created item: %s"), *ItemName[IconIndex]);  // ������ ���� �α�

	//	mInventory->AddItem(ItemData);
	//}
	//for (int32 i = 0; i < 3; ++i) //Temporary Test Item AddCode
	//{
	//	UInventoryItemData* ItemData = NewObject<UInventoryItemData>();

	//	int32	IconIndex = FMath::RandRange(0, 2);

	//	ItemData->SetInfoFrom(IconPath[IconIndex], ItemName[IconIndex], 1);

	//	mInventory->AddItem(ItemData);
	//}
	
}

void USagaInventoryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void USagaInventoryWidget::OnListItemHover(UObject* Item, bool IsHovered)
{
	/*UInventoryItemData* ItemData = Cast<UInventoryItemData>(Item);

	if (IsValid(ItemData) && mSelectionItem != ItemData)
	{
		ItemData->SetMouseOn(IsHovered);

		USagaInventoryListWidget* EntryWidget = mInventory->GetEntryWidgetFromItem<USagaInventoryListWidget>(Item);

		if (IsValid(EntryWidget))
		{
			if (IsHovered)
			{
				EntryWidget->SetMouseState(EEntryWidgetMouseState::MouseOn);
			}
			else
			{
				EntryWidget->SetMouseState(EEntryWidgetMouseState::None);
			}
		}
	}*/
}

void USagaInventoryWidget::OnListItemSelection(UObject* Item)
{
	/*if (IsValid(mSelectionItem))
	{
		mSelectionItem->SetSelect(false);
		USagaInventoryListWidget* EntryWidget = mInventory->GetEntryWidgetFromItem<USagaInventoryListWidget>(mSelectionItem);
		if (IsValid(EntryWidget))
		{
			EntryWidget->SetMouseState(EEntryWidgetMouseState::None);
		}
	}

	mSelectionItem = Cast<UInventoryItemData>(Item);

	if (IsValid(mSelectionItem))
	{
		mSelectionItem->SetSelect(true);
		USagaInventoryListWidget* EntryWidget = mInventory->GetEntryWidgetFromItem<USagaInventoryListWidget>(mSelectionItem);
		if (IsValid(EntryWidget))
		{
			EntryWidget->SetMouseState(EEntryWidgetMouseState::Select);
		}
	}*/
}

void
USagaInventoryWidget::OnListItemClick(UObject* Item)
{
	/*UInventoryItemData* entry = Cast<UInventoryItemData>(Item);*/

//	if (IsValid(entry))
//	{
//		//FString ItemName = entry->GetItemName();
//
//#if WITH_EDITOR
//
//		//UE_LOG(LogSagaGame, Log, TEXT("[USagaInventoryWidget] Item clicked: %s"), *ItemName);
//#endif
//		/*UseGumball(entry);
//		UseEnergyDrink(entry);
//		UseSmokeBomb(entry);*/
//	}
}

void
USagaInventoryWidget::UseEnergyDrink()
{
	const auto world = GetWorld();
	const auto net = USagaNetworkSubSystem::GetSubSystem(world);

	if (net->IsOfflineMode())
	{
		const APlayerController* const pc = GetOwningPlayer();

		if (IsValid(pc))
		{
			const auto pawn = pc->GetPawn<ASagaCharacterBase>();

			if (IsValid(pawn))
			{
				UE_LOG(LogSagaGame, Log, TEXT("[USagaInventoryWidget] Using Energy Drink (Offline Mode)"));

				pawn->ExecuteUseItem(ESagaItemTypes::Drink);
			}
			else
			{
				UE_LOG(LogSagaGame, Error, TEXT("[USagaInventoryWidget] There is no pawn. (Offline Mode)"));
			}
		}
		else
		{
			UE_LOG(LogSagaGame, Error, TEXT("[USagaInventoryWidget] There is no controller. (Offline Mode)"));
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Log, TEXT("[USagaInventoryWidget] Energy Drink RPC"));

		net->SendRpcPacket(ESagaRpcProtocol::RPC_USE_ITEM_0, 0, 0);
	}

	//mInventory->RemoveItem(item_entry);
}

void
USagaInventoryWidget::UseGumball()
{
	const auto world = GetWorld();
	const auto net = USagaNetworkSubSystem::GetSubSystem(world);

	if (net->IsOfflineMode())
	{
		const APlayerController* const pc = GetOwningPlayer();

		if (IsValid(pc))
		{
			const auto pawn = pc->GetPawn<ASagaCharacterBase>();

			if (IsValid(pawn))
			{
				UE_LOG(LogSagaGame, Log, TEXT("[USagaInventoryWidget] Using Energy Drink (Offline Mode)"));

				pawn->ExecuteUseItem(ESagaItemTypes::Gum);
			}
			else
			{
				UE_LOG(LogSagaGame, Error, TEXT("[USagaInventoryWidget] There is no pawn. (Offline Mode)"));
			}
		}
		else
		{
			UE_LOG(LogSagaGame, Error, TEXT("[USagaInventoryWidget] There is no controller. (Offline Mode)"));
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Log, TEXT("[USagaInventoryWidget] Gumball RPC"));

		net->SendRpcPacket(ESagaRpcProtocol::RPC_USE_ITEM_0, 0, 1);
	}

	//mInventory->RemoveItem(item_entry);
}

void
USagaInventoryWidget::UseSmokeBomb()
{
	const auto world = GetWorld();
	const auto net = USagaNetworkSubSystem::GetSubSystem(world);

	if (net->IsOfflineMode())
	{
		const APlayerController* const pc = GetOwningPlayer();

		if (IsValid(pc))
		{
			const auto pawn = pc->GetPawn<ASagaCharacterBase>();

			if (IsValid(pawn))
			{
				UE_LOG(LogSagaGame, Log, TEXT("[USagaInventoryWidget] Smoke Bomb (Offline Mode)"));

				pawn->ExecuteUseItem(ESagaItemTypes::SmokeBomb);
			}
			else
			{
				UE_LOG(LogSagaGame, Error, TEXT("[USagaInventoryWidget] There is no pawn. (Offline Mode)"));
			}
		}
		else
		{
			UE_LOG(LogSagaGame, Error, TEXT("[USagaInventoryWidget] There is no controller. (Offline Mode)"));
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Log, TEXT("[USagaInventoryWidget] Smoke Bomb RPC"));

		net->SendRpcPacket(ESagaRpcProtocol::RPC_USE_ITEM_0, 0, 2);
	}

	//mInventory->RemoveItem(item_entry);
}

void
USagaInventoryWidget::AddItemToInventory(UInventoryItemData* ItemData)
{
	if (mInventory)
	{
		mInventory->AddItem(ItemData);
	}
}
