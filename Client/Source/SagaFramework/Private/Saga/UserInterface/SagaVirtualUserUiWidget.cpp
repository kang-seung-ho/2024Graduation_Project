#include "Saga/Interface/SagaVirtualUserUiWidget.h"
#include <UObject/Object.h>
#include <Blueprint/IUserObjectListEntry.h>

#include "Saga/Interface/Data/SagaUiInstancedUserData.h"

void
USagaVirtualUserUiWidget::NativeOnListItemObjectSet(UObject* data)
{
	const auto room_data = Cast<USagaUiInstancedUserData>(data);

	if (IsValid(room_data))
	{
		myDataHolder = room_data;
	}
	else
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaVirtualUserUiWidget] The provied user data is null."));
	}

	IUserObjectListEntry::NativeOnListItemObjectSet(data);
}

void
USagaVirtualUserUiWidget::NativeOnItemSelectionChanged(bool is_selected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(is_selected);

	if (IsValid(myDataHolder))
	{
		myDataHolder->isSelected = is_selected;
	}
}

void
USagaVirtualUserUiWidget::NativeOnEntryReleased()
{
	IUserObjectListEntry::NativeOnEntryReleased();

	if (IsValid(myDataHolder))
	{
		myDataHolder->isCursorHovered = false;
		myDataHolder->isSelected = false;
	}
}

FText
USagaVirtualUserUiWidget::GetNicknameLabelText()
const
{
	if (IsValid(myDataHolder))
	{
		return myDataHolder->virtualMember.myName;
	}
	else
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaVirtualUserUiWidget] User data is null."));

		return FText::GetEmpty();
	}
}

FText
USagaVirtualUserUiWidget::GetUserIdLabelText()
const
{
	if (IsValid(myDataHolder))
	{
		return FText::AsNumber(myDataHolder->virtualMember.myID);
	}
	else
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaVirtualUserUiWidget] User data is null."));

		return FText::GetEmpty();
	}
}
