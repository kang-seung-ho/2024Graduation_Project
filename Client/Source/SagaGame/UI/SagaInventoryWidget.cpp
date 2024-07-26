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

void
USagaInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	mCloseButton = Cast<UButton>(GetWidgetFromName(TEXT("CloseButton")));

	if (mCloseButton)
	{
		mCloseButton->OnClicked.AddDynamic(this, &USagaInventoryWidget::CloseButtonClick);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CloseButton not found!"));
	}

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
}

void USagaInventoryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void USagaInventoryWidget::CloseButtonClick()
{
	UE_LOG(LogTemp, Warning, TEXT("CloseButton clicked"));
	SetVisibility(ESlateVisibility::Collapsed);

	APlayerController* Controller = GetOwningPlayer();
	if (Controller)
	{
		ASagaInGamePlayerController* Character = Cast<ASagaInGamePlayerController>(Controller);
		if (Character)
		{
			Character->SetInventoryVisibility(false);
		}
	}
}

void USagaInventoryWidget::OnListItemHover(UObject* Item, bool IsHovered)
{
	UInventoryItemData* ItemData = Cast<UInventoryItemData>(Item);

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
	}
}

void USagaInventoryWidget::OnListItemSelection(UObject* Item)
{
	if (IsValid(mSelectionItem))
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
	}
}

void
USagaInventoryWidget::OnListItemClick(UObject* Item)
{
	UInventoryItemData* entry = Cast<UInventoryItemData>(Item);

	if (IsValid(entry))
	{
#if WITH_EDITOR

		FString ItemName = entry->GetItemName();
		UE_LOG(LogSagaGame, Log, TEXT("[USagaInventoryWidget] Item clicked: %s"), *ItemName);
#endif

		if (ItemName == "EnergyDrink")
		{
			UseEnergyDrink(entry);
		}
		else if (ItemName == "Gumball")
		{
			UseGumball(entry);
		}
		else if (ItemName == "SmokeBomb")
		{
			UseSmokeBomb(entry);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("[USagaInventoryWidget] There is no item '%s'."), *ItemName);
		}

		//if (net->IsOfflineMode())
		{

		}
		/*
		else
		{
			if (ItemName == "EnergyDrink")
			{
				net->SendRpcPacket(ESagaRpcProtocol::RPC_USE_ITEM_0, index, 0);
			}
			else if (ItemName == "Gumball")
			{
				net->SendRpcPacket(ESagaRpcProtocol::RPC_USE_ITEM_0, index, 1);
			}
			else if (ItemName == "SmokeBomb")
			{
				net->SendRpcPacket(ESagaRpcProtocol::RPC_USE_ITEM_0, index, 2);
			}
		}*/
	}
}

void
USagaInventoryWidget::UseEnergyDrink(UInventoryItemData* item_entry)
{
	const auto index = mInventory->GetIndexForItem(item_entry);
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

		net->SendRpcPacket(ESagaRpcProtocol::RPC_USE_ITEM_0, index, 0);
	}

	mInventory->RemoveItem(item_entry);
}

void
USagaInventoryWidget::UseGumball(UInventoryItemData* item_entry)
{
	const auto index = mInventory->GetIndexForItem(item_entry);
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

		net->SendRpcPacket(ESagaRpcProtocol::RPC_USE_ITEM_0, index, 1);
	}

	mInventory->RemoveItem(item_entry);
}

void
USagaInventoryWidget::UseSmokeBomb(UInventoryItemData* item_entry)
{
	const auto index = mInventory->GetIndexForItem(item_entry);
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

		net->SendRpcPacket(ESagaRpcProtocol::RPC_USE_ITEM_0, index, 2);
	}

	mInventory->RemoveItem(item_entry);
}

void
USagaInventoryWidget::AddItemToInventory(UInventoryItemData* ItemData)
{
	if (mInventory)
	{
		mInventory->AddItem(ItemData);
	}
}
