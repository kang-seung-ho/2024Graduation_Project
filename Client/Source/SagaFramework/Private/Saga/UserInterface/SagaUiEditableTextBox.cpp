#include "Saga/Interface/SagaUiEditableTextBox.h"
#include <UObject/ConstructorHelpers.h>
#include <Internationalization/Text.h>
#include <Components/TextWidgetTypes.h>
#include <Blueprint/WidgetTree.h>
#include <IAudioModulation.h>
#include <Fonts/SlateFontInfo.h>
#include <Styling/DefaultStyleCache.h>

USagaUiEditableTextBox::USagaUiEditableTextBox(const FObjectInitializer& initializer)
	: Super(initializer)
	, myEditableTextBlock()
	, hintText(), hintTextDelegate()
	, OnTextChanged(), OnTextCommitted()
{}

void
USagaUiEditableTextBox::NativePreConstruct()
{
	Super::NativePreConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (auto box = Cast<UEditableTextBox>(element))
			{
				// myEditableTextBlock in editor
				if (hintTextDelegate.IsBound())
				{
					box->SetHintText(hintTextDelegate.Execute());
				}
				else
				{
					box->SetHintText(hintText);
				}
			}
		}
	);
}

void
USagaUiEditableTextBox::NativeConstruct()
{
	Super::NativeConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (auto widget = Cast<UEditableTextBox>(element))
			{
				myEditableTextBlock = widget;
			}
		}
	);

	const auto my_name = GetName();
	if (IsValid(myEditableTextBlock))
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaUiEditableTextBox] '%s' found text box in children."), *my_name);

		if (hintTextDelegate.IsBound())
		{
			myEditableTextBlock->SetHintText(hintTextDelegate.Execute());
			myEditableTextBlock->HintTextDelegate = hintTextDelegate;
		}
		else
		{
			myEditableTextBlock->SetHintText(hintText);
		}

		myEditableTextBlock->OnTextChanged.AddDynamic(this, &USagaUiEditableTextBox::HandleOnTextChanged);
		myEditableTextBlock->OnTextCommitted.AddDynamic(this, &USagaUiEditableTextBox::HandleOnTextCommitted);
	}
	else
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaUiEditableTextBox] '%s' found no text box in children."), *my_name);
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

void
USagaUiEditableTextBox::SetText(FText text)
noexcept
{
	if (nullptr != myEditableTextBlock)
	{
		myEditableTextBlock->SetText(text);
	}
}

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
	if (nullptr != myEditableTextBlock)
	{
		myEditableTextBlock->SetJustification(justification);
	}
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
	if (nullptr != myEditableTextBlock)
	{
		myEditableTextBlock->SetTextOverflowPolicy(policy);
	}
}

ETextOverflowPolicy
USagaUiEditableTextBox::GetTextOverflowPolicy()
const noexcept
{
	return myEditableTextBlock->GetTextOverflowPolicy();
}
