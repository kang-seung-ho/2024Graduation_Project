#include "Saga/Interface/SagaUiButton.h"
#include <Binding/States/WidgetStateRegistration.h>
#include <Blueprint/WidgetTree.h>

USagaUiButton::USagaUiButton(const FObjectInitializer& initializer)
	: Super(initializer)
	, myButton(), myLabel()
	, buttonCaption(FText::FromString(TEXT("Button")))
{}

void
USagaUiButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (element == this) return;

			if (auto btn = Cast<UButton>(element))
			{
				myButton = btn;
			}
			else if (auto lbl = Cast<UTextBlock>(element))
			{
				myLabel = lbl;
			}
		}
	);

	const auto my_name = GetName();
	if (nullptr == myButton)
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaUiButton] '%s' found no slate button in children."), *my_name);

	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaUiButton] '%s' found slate button in children."), *my_name);

		myButton->OnClicked.AddUniqueDynamic(this, &USagaUiButton::HandleClicked);
		myButton->OnPressed.AddUniqueDynamic(this, &USagaUiButton::HandlePressed);
		myButton->OnReleased.AddUniqueDynamic(this, &USagaUiButton::HandleReleased);
		myButton->OnHovered.AddUniqueDynamic(this, &USagaUiButton::HandleHovered);
		myButton->OnUnhovered.AddUniqueDynamic(this, &USagaUiButton::HandleUnhovered);
	}

	if (nullptr == myLabel)
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaUiButton] '%s' found no label in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaUiButton] '%s' found label in children."), *my_name);
		myLabel->SetText(buttonCaption);
	}
}

void
USagaUiButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (nullptr != myLabel)
	{
		myLabel->SetText(buttonCaption);
	}
}

void
USagaUiButton::SetText(FText text)
noexcept
{
	buttonCaption = text;
	myLabel->SetText(text);
}

FText
USagaUiButton::GetText()
const noexcept
{
	return myLabel->GetText();
}

void
USagaUiButton::HandleClicked()
{
	if (isUserInteractable)
	{
		OnClicked.Broadcast();
	}
}

void
USagaUiButton::HandlePressed()
{
	if (isUserInteractable)
	{
		OnPressed.Broadcast();
		BroadcastBinaryPostStateChange(UWidgetPressedStateRegistration::Bit, true);
	}
}

void
USagaUiButton::HandleReleased()
{
	if (isUserInteractable)
	{
		OnReleased.Broadcast();
		BroadcastBinaryPostStateChange(UWidgetPressedStateRegistration::Bit, false);
	}
}

void
USagaUiButton::HandleHovered()
{
	if (isUserInteractable)
	{
		OnHovered.Broadcast();
		BroadcastBinaryPostStateChange(UWidgetHoveredStateRegistration::Bit, true);
	}
}

void
USagaUiButton::HandleUnhovered()
{
	if (isUserInteractable)
	{
		OnUnhovered.Broadcast();
		BroadcastBinaryPostStateChange(UWidgetHoveredStateRegistration::Bit, false);
	}
}
