#include "Saga/Interface/SagaUiPopup.h"
#include <Engine/Font.h>
#include <UObject/UObjectGlobals.h>
#include <Math/Color.h>
#include <Styling/SlateColor.h>
#include <Blueprint/WidgetTree.h>

USagaUiPopup::USagaUiPopup(const FObjectInitializer& initializer)
	: Super(initializer)
	, myTitle(), myText()
	, popupTitle(FText::FromString(TEXT("Title"))), popupTitleDelegate()
	, popupContentText(FText::FromString(TEXT("-"))), popupContentTextDelegate()
	, titleColorAndOpacity(FLinearColor{ 0.039216f, 0.101961f, 0.062745 }), titleColorAndOpacityDelegate()
	, textColorAndOpacity(FLinearColor::Black), textColorAndOpacityDelegate()
{}

void
USagaUiPopup::SetTitle(FText text)
noexcept
{
	popupTitle = text;

	if (nullptr != myTitle)
	{
		myTitle->SetText(text);
	}
}

FText
USagaUiPopup::GetTitle()
const noexcept
{
	return popupTitle;
}

void
USagaUiPopup::SetText(FText text)
noexcept
{
	popupContentText = text;

	if (nullptr != myText)
	{
		myText->SetText(text);
	}
}

FText
USagaUiPopup::GetText()
const noexcept
{
	return popupContentText;
}

void
USagaUiPopup::SetTitleColorAndOpacity(FSlateColor colour)
noexcept
{
	titleColorAndOpacity = colour;

	if (nullptr != myTitle)
	{
		myTitle->SetColorAndOpacity(colour);
	}
}

FSlateColor
USagaUiPopup::GetTitleColorAndOpacity()
const noexcept
{
	return titleColorAndOpacity;
}

void
USagaUiPopup::SetTitleOpacity(float opacity)
noexcept
{
	auto color = titleColorAndOpacity.GetSpecifiedColor();
	color.A = opacity;

	SetTitleColorAndOpacity(FSlateColor{ color });
}

void
USagaUiPopup::SetTextColorAndOpacity(FSlateColor colour)
noexcept
{
	textColorAndOpacity = colour;

	if (nullptr != myText)
	{
		myText->SetColorAndOpacity(colour);
	}
}

FSlateColor
USagaUiPopup::GetTextColorAndOpacity()
const noexcept
{
	return textColorAndOpacity;
}

void
USagaUiPopup::SetTextOpacity(float opacity)
noexcept
{
	auto color = textColorAndOpacity.GetSpecifiedColor();
	color.A = opacity;

	SetTextColorAndOpacity(FSlateColor{ color });
}

void
USagaUiPopup::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (element == this) return;

			if (auto labelwidget = Cast<UTextBlock>(element))
			{
				if (labelwidget->GetName() == TEXT("Title"))
				{
					myTitle = labelwidget;
				}
				else
				{
					myText = labelwidget;
				}
			}
		}
	);

	const auto my_name = GetName();
	if (nullptr == myTitle)
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaUiPopup] '%s' found no title text in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaUiPopup] '%s' found the title text in children."), *my_name);
	}

	if (nullptr == myText)
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaUiPopup] '%s' found no content text in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaUiPopup] '%s' found the content text in children."), *my_name);
	}
}

void
USagaUiPopup::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (nullptr != myTitle)
	{
		myTitle->SetText(popupTitle);
		myTitle->SetColorAndOpacity(titleColorAndOpacity);
	}

	if (nullptr != myText)
	{
		myText->SetText(popupContentText);
		myText->SetColorAndOpacity(textColorAndOpacity);
	}
}
