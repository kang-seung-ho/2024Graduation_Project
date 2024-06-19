#include "UI/SagaInventoryWidget.h"
#include "../PlayerControllers/SagaInGamePlayerController.h"
#include "Components/Button.h"
#include "InventoryItemData.h"
#include "SagaInventoryListWidget.h"
#include "Item/Gumball.h"

void USagaInventoryWidget::NativeConstruct()
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

    for (int32 i = 0; i < 10; ++i)
    {
        UInventoryItemData* ItemData = NewObject<UInventoryItemData>();

        int32 IconIndex = FMath::RandRange(0, 2);
        UTexture2D* IconTexture = LoadObject<UTexture2D>(nullptr, *IconPath[IconIndex]);

        ItemData->SetInfo(IconTexture, ItemName[IconIndex], 1);

        UE_LOG(LogTemp, Warning, TEXT("Created item: %s"), *ItemName[IconIndex]);  // 아이템 생성 로그

        mInventory->AddItem(ItemData);
    }
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

void USagaInventoryWidget::OnListItemClick(UObject* Item)
{
    UInventoryItemData* ItemData = Cast<UInventoryItemData>(Item);
    if (IsValid(ItemData))
    {
        FString ItemName = ItemData->GetItemName();
        UE_LOG(LogTemp, Warning, TEXT("Item clicked: %s"), *ItemName);

        if (ItemName == "EnergyDrink")
        {
            UseEnergyDrink();
        }
        else if (ItemName == "Gumball")
        {
            UseGumball();
        }
        else if (ItemName == "SmokeBomb")
        {
            UseSmokeBomb();
        }

        mInventory->RemoveItem(ItemData);
    }
}

void USagaInventoryWidget::UseEnergyDrink()
{
    UE_LOG(LogTemp, Warning, TEXT("Using Energy Drink"));  // 함수 호출 로그
    
    //플레이어 HP 회복 로직 추가
    // + 플레이어 HP회복 시 이펙트 재생 추가


}

void USagaInventoryWidget::UseGumball()
{
    UE_LOG(LogTemp, Warning, TEXT("Using Gumball"));  // 함수 호출 로그


    // 껌볼 사용 로직 

    APlayerController* PlayerController = GetOwningPlayer();
    if (PlayerController)
    {
        APawn* PlayerPawn = PlayerController->GetPawn();
        if (PlayerPawn)
        {
            FVector SpawnLocation = PlayerPawn->GetActorLocation() + PlayerPawn->GetActorForwardVector() * 200.0f;  // 플레이어 앞 200 유닛
            FRotator SpawnRotation = PlayerPawn->GetActorRotation();
            FActorSpawnParameters SpawnParams;
            GetWorld()->SpawnActor<AGumball>(AGumball::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
        }
    }
}

void USagaInventoryWidget::UseSmokeBomb()
{
    UE_LOG(LogTemp, Warning, TEXT("Using Smoke Bomb"));  // 함수 호출 로그
    // 연막탄 사용 로직 추가
}
