#include "Saga/Interface/SagaUiEditableTextBox.h"
#include <UObject/ConstructorHelpers.h>
#include <Internationalization/Text.h>
#include <Components/TextWidgetTypes.h>
#include <Blueprint/WidgetTree.h>
#include <IAudioModulation.h>
#include <Fonts/SlateFontInfo.h>
#include <Styling/DefaultStyleCache.h>

#define LOCTEXT_NAMESPACE "UMG"

USagaUiEditableTextBox::USagaUiEditableTextBox(const FObjectInitializer& initializer)
	: Super(initializer)
	, myEditableTextBlock()
	, hintText()
	, OnTextChanged(), OnTextCommitted()
{}

FText
USagaUiEditableTextBox::GetText()
const noexcept
{
	return myEditableTextBlock->GetText();
}

void
USagaUiEditableTextBox::SetReadOnly(bool flag)
noexcept
{
	myEditableTextBlock->SetIsReadOnly(flag);
}

bool
USagaUiEditableTextBox::IsReadOnly()
const noexcept
{
	return myEditableTextBlock->GetIsReadOnly();
}

void
USagaUiEditableTextBox::SetHintText(FText text)
noexcept
{
	hintText = text;

	if (nullptr != myEditableTextBlock)
	{
		myEditableTextBlock->SetHintText(text);
	}
}

FText
USagaUiEditableTextBox::GetHintText()
const noexcept
{
	return hintText;
}

void
USagaUiEditableTextBox::SetJustification(ETextJustify::Type justification)
noexcept
{
	myEditableTextBlock->SetJustification(justification);
}

ETextJustify::Type
USagaUiEditableTextBox::GetJustification()
const noexcept
{
	return myEditableTextBlock->GetJustification();
}

void
USagaUiEditableTextBox::SetTextOverflowPolicy(ETextOverflowPolicy policy)
noexcept
{
	myEditableTextBlock->SetTextOverflowPolicy(policy);
}

ETextOverflowPolicy
USagaUiEditableTextBox::GetTextOverflowPolicy()
const noexcept
{
	return myEditableTextBlock->GetTextOverflowPolicy();
}

void
USagaUiEditableTextBox::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (element == this) return;

			if (auto widget = Cast<UEditableTextBox>(element))
			{
				myEditableTextBlock = widget;
			}
		}
	);

	const auto my_name = GetName();
	if (nullptr == myEditableTextBlock)
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaUiEditableTextBox] '%s' found no text box in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaUiEditableTextBox] '%s' found text box in children."), *my_name);

		myEditableTextBlock->OnTextChanged.AddUniqueDynamic(this, &USagaUiEditableTextBox::HandleOnTextChanged);
		myEditableTextBlock->OnTextCommitted.AddUniqueDynamic(this, &USagaUiEditableTextBox::HandleOnTextCommitted);
	}
}

void
USagaUiEditableTextBox::NativePreConstruct()
{
	if (nullptr != myEditableTextBlock)
	{
		myEditableTextBlock->SetHintText(hintText);
	}
}

void
USagaUiEditableTextBox::HandleOnTextChanged(const FText& text)
{
	OnTextChanged.Broadcast(text);
}

void
USagaUiEditableTextBox::HandleOnTextCommitted(const FText& text, ETextCommit::Type method)
{
	OnTextCommitted.Broadcast(text, method);
}
