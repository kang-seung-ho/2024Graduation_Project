#include "Saga/Interface/SagaUiPopup.h"
#include <Math/UnrealMathUtility.h>
#include <Engine/Font.h>
#include <UObject/UObjectGlobals.h>
#include <Math/Color.h>
#include <Slate/WidgetTransform.h>
#include <Styling/SlateColor.h>
#include <Blueprint/WidgetTree.h>

#include "Saga/Interface/SagaUiPanelWidget.h"

USagaUiPopup::USagaUiPopup(const FObjectInitializer& initializer)
	: Super(initializer)
	, myBody(), myTitle(), myText()
	, popupTitle(FText::FromString(TEXT("Title"))), popupTitleDelegate()
	, popupContentText(FText::FromString(TEXT("-"))), popupContentTextDelegate()
	, titleColorAndOpacity(FLinearColor{ 0.039216f, 0.101961f, 0.062745 }), titleColorAndOpacityDelegate()
	, textColorAndOpacity(FLinearColor::Black), textColorAndOpacityDelegate()
{
	SetTransitionPeriod(ESagaLiveUserWidgetStates::Opening, 0.6f);
	SetTransitionPeriod(ESagaLiveUserWidgetStates::Closing, 1.2f);
}

void
USagaUiPopup::NativePreConstruct()
{
	Super::NativePreConstruct();

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
			else if (auto panelwidget = Cast<USagaUiPanelWidget>(element))
			{
				myBody = panelwidget;
			}
		}
	);

	const auto my_name = GetName();
	if (nullptr == myBody)
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaUiPopup] '%s' found no panel widget in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaUiPopup] '%s' found panel widget in children."), *my_name);
	}

	if (nullptr == myTitle)
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaUiPopup] '%s' found no title text in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaUiPopup] '%s' found the title text in children."), *my_name);

		myTitle->SetText(popupTitle);
		myTitle->SetColorAndOpacity(titleColorAndOpacity);
	}

	if (nullptr == myText)
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaUiPopup] '%s' found no content text in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaUiPopup] '%s' found the content text in children."), *my_name);

		myText->SetText(popupContentText);
		myText->SetColorAndOpacity(textColorAndOpacity);
	}
}

void
USagaUiPopup::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	OnClosed.AddDynamic(this, &USagaUiPopup::HandleOnClosed);
}

void
USagaUiPopup::NativeTick(const FGeometry& geometry, float delta_time)
{
	Super::NativeTick(geometry, delta_time);
}

void
USagaUiPopup::HandleOnClosed()
{
	RemoveFromParent();
}

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
