#include "Saga/Interface/SagaVirtualRoomUiWidget.h"
#include <UObject/Object.h>
#include <Blueprint/IUserObjectListEntry.h>

#include "Saga/Interface/Data/SagaUiInstancedRoomData.h"

USagaVirtualRoomUiWidget::USagaVirtualRoomUiWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, myDataHolder()
{}

void
USagaVirtualRoomUiWidget::NativeOnListItemObjectSet(UObject* data)
{
	const auto room_data = Cast<USagaUiInstancedRoomData>(data);

	if (IsValid(room_data))
	{
		myDataHolder = room_data;
	}
	else
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaVirtualUserUiWidget] The provied room data is null."));
	}

	IUserObjectListEntry::NativeOnListItemObjectSet(data);
}

void
USagaVirtualRoomUiWidget::NativeOnItemSelectionChanged(bool is_selected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(is_selected);

	if (IsValid(myDataHolder))
	{
		myDataHolder->isSelected = is_selected;
	}
}

void
USagaVirtualRoomUiWidget::NativeOnEntryReleased()
{
	IUserObjectListEntry::NativeOnEntryReleased();

	if (IsValid(myDataHolder))
	{
		myDataHolder->isCursorHovered = false;
		myDataHolder->isSelected = false;
	}
}

FText
USagaVirtualRoomUiWidget::GetTitleLabelText()
const
{
	if (IsValid(myDataHolder))
	{
		return myDataHolder->virtualRoom.myName;
	}
	else
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaVirtualRoomUiWidget] Room data is null."));

		return FText::GetEmpty();
	}
}

FText
USagaVirtualRoomUiWidget::GetMemberCountLabelText()
const
{
	if (IsValid(myDataHolder))
	{
		const auto cnt = myDataHolder->virtualRoom.MembersCount;
		const auto fmt = FString::Printf(TEXT("(%d/4)"), cnt);

		return FText::FromString(fmt);
	}
	else
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaVirtualRoomUiWidget] Room data is null."));

		return FText::FromString(TEXT("(X)"));
	}
}
