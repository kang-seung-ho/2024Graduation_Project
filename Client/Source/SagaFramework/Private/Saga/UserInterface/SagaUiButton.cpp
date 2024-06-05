#include "Saga/Interface/SagaUiButton.h"
#include <UObject/UObjectGlobals.h>
#include <UObject/Object.h>
#include <Binding/States/WidgetStateRegistration.h>
#include <Blueprint/WidgetTree.h>

USagaUiButton::USagaUiButton(const FObjectInitializer& initializer)
	: Super(initializer)
	, myButton(), myLabel()
	, buttonCaption(FText::FromString(TEXT("Button"))), buttonCaptionDelegate()
{
	myTickPolicy = ESagaLiveUserWidgetTickPolicy::Both;
}

void
USagaUiButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (element == this) return;

			if (auto btn = Cast<UButton>(element))
			{
				// myButton in editor
				if (isUserInteractableDelegate.IsBound())
				{
					//btn->SetIsEnabled(isUserInteractableDelegate.Execute());
				}
				else
				{
					//btn->SetIsEnabled(isUserInteractable);
				}
			}
			else if (auto lbl = Cast<UTextBlock>(element))
			{
				// myLabel in editor
				if (buttonCaptionDelegate.IsBound())
				{
					lbl->SetText(buttonCaptionDelegate.Execute());
				}
				else
				{
					lbl->SetText(buttonCaption);
				}
			}
		}
	);

}

void
USagaUiButton::NativeConstruct()
{
	Super::NativeConstruct();

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
	if (IsValid(myButton))
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaUiButton] '%s' found slate button in children."), *my_name);

		if (isUserInteractableDelegate.IsBound())
		{
			//myButton->SetIsEnabled(isUserInteractableDelegate.Execute());
			//myButton->bIsEnabledDelegate = isUserInteractableDelegate;
		}
		else
		{
			//myButton->SetIsEnabled(isUserInteractable);
		}

		myButton->OnClicked.AddDynamic(this, &USagaUiButton::HandleClicked);
		myButton->OnPressed.AddDynamic(this, &USagaUiButton::HandlePressed);
		myButton->OnReleased.AddDynamic(this, &USagaUiButton::HandleReleased);
		myButton->OnHovered.AddDynamic(this, &USagaUiButton::HandleHovered);
		myButton->OnUnhovered.AddDynamic(this, &USagaUiButton::HandleUnhovered);
	}
	else
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaUiButton] '%s' found no slate button in children."), *my_name);
	}

	if (IsValid(myLabel))
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaUiButton] '%s' found label in children."), *my_name);

		if (buttonCaptionDelegate.IsBound())
		{
			myLabel->SetText(buttonCaptionDelegate.Execute());
			myLabel->TextDelegate = buttonCaptionDelegate;
		}
		else
		{
			myLabel->SetText(buttonCaption);
		}
	}
	else
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaUiButton] '%s' found no label in children."), *my_name);
	}
}

void
USagaUiButton::SetText(FText text)
noexcept
{
	buttonCaption = text;
}

FText
USagaUiButton::GetText()
const noexcept
{
	return buttonCaption;
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
