// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SagaInventoryListWidget.h"
#include "InventoryItemData.h"



void USagaInventoryListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	mIconImage = Cast<UImage>(GetWidgetFromName(TEXT("Icon")));
	mSelection = Cast<UImage>(GetWidgetFromName(TEXT("Selection")));
	//mNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("NameText")));
	mCountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CountText")));

	if (!mSelection)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selection widget not found!"));
	}
}

void USagaInventoryListWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void USagaInventoryListWidget::SetMouseState(EEntryWidgetMouseState State)
{
	switch (State)
	{
	case EEntryWidgetMouseState::None:
		mSelection->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
		break;
	case EEntryWidgetMouseState::MouseOn:
		mSelection->SetColorAndOpacity(FLinearColor(1.f, 1.f, 0.f, 0.3f));
		break;
	case EEntryWidgetMouseState::Select:
		mSelection->SetColorAndOpacity(FLinearColor(0.f, 0.f, 1.f, 0.3f));
		break;
	}
}

//인터페이스를 추가하며 생성된 OnListItemObjectSet함수에서 이 함수를 호출해주고 있음.
//리스트의 목록이 갱신되면 갱신된 목록에 추가될 데이터 object를 인자로 전달. 리스트에 추가된 데이터를 객체를 전달한다는 것.
//처음 생성될 시, Construct 함수먼저 호출 --> 그다음에 SetItem함수 호출
void USagaInventoryListWidget::SetItem(UObject* Item)
{
	UInventoryItemData* ItemData = Cast<UInventoryItemData>(Item);

	if (IsValid(ItemData))
	{
		mData = ItemData;

		mIconImage->SetBrushFromTexture(ItemData->GetIconTexture());
		//mNameText->SetText(FText::FromString(ItemData->GetItemName()));
		mCountText->SetText(FText::FromString(FString::FromInt(ItemData->GetCount())));

		if (mData->IsSelect())
		{
			SetMouseState(EEntryWidgetMouseState::Select);
		}

		else if (mData->IsMouseOn())
		{
			SetMouseState(EEntryWidgetMouseState::MouseOn);
		}

		else
		{
			SetMouseState(EEntryWidgetMouseState::None);
		}
	}
}
