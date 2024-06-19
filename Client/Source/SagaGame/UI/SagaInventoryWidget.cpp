#include "UI/SagaInventoryWidget.h"
#include "../PlayerControllers/SagaInGamePlayerController.h"
#include "Components/Button.h"
#include "InventoryItemData.h"

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

    FString IconPath[3] =
    {
        TEXT("/Script/Engine.Texture2D'/Game/UI/Images/Tex_heart.Tex_heart'"),
        TEXT("/Script/Engine.Texture2D'/Game/UI/Images/gumball_machine.gumball_machine'"),
        TEXT("/Script/Engine.Texture2D'/Game/UI/Images/smoke_bomb.smoke_bomb'")
    };

    FString ItemName[3] =
    {
        TEXT("EnergyDrink"),
        TEXT("GumballMachine"),
        TEXT("SmokeBomb")
    };

    for (int32 i = 0; i < 10; ++i)
    {
        UInventoryItemData* ItemData = NewObject<UInventoryItemData>();

        int32 IconIndex = FMath::RandRange(0, 2);
        UTexture2D* IconTexture = LoadObject<UTexture2D>(nullptr, *IconPath[IconIndex]);

        ItemData->SetInfo(IconTexture, ItemName[IconIndex], 1);

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
