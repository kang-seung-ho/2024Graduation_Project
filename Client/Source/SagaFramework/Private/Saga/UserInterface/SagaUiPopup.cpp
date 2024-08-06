#include "Saga/Interface/SagaUiPopup.h"
#include <Engine/Font.h>
#include <UObject/UObjectGlobals.h>
#include <Math/UnrealMathUtility.h>
#include <Math/Color.h>
#include <Slate/WidgetTransform.h>
#include <Components/Widget.h>
#include <Components/TextBlock.h>
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
	SetTransitionPeriod(ESagaLiveUserWidgetStates::Closing, 0.8f);
}

void
USagaUiPopup::NativePreConstruct()
{
	Super::NativePreConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (auto text = Cast<UTextBlock>(element))
			{
				if (text->GetName() == TEXT("Title"))
				{
					// myTitle in editor
					if (popupTitleDelegate.IsBound())
					{
						text->SetText(popupTitleDelegate.Execute());
					}
					else
					{
						text->SetText(popupTitle);
					}

					if (titleColorAndOpacityDelegate.IsBound())
					{
						text->SetColorAndOpacity(titleColorAndOpacityDelegate.Execute());
					}
					else
					{
						text->SetColorAndOpacity(titleColorAndOpacity);
					}
				}
				else
				{
					// myText in editor
					if (popupContentTextDelegate.IsBound())
					{
						text->SetText(popupContentTextDelegate.Execute());
					}
					else
					{
						text->SetText(popupContentText);
					}

					if (textColorAndOpacityDelegate.IsBound())
					{
						text->SetColorAndOpacity(textColorAndOpacityDelegate.Execute());
					}
					else
					{
						text->SetColorAndOpacity(textColorAndOpacity);
					}
				}
			}
			else if (auto panelwidget = Cast<USagaUiPanelWidget>(element))
			{
				// myBody in editor

			}
		}
	);
}

void
USagaUiPopup::NativeConstruct()
{
	Super::NativeConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
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
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaUiPopup] '%s' found no panel widget in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaUiPopup] '%s' found panel widget in children."), *my_name);
	}

	if (nullptr == myTitle)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaUiPopup] '%s' found no title text in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaUiPopup] '%s' found the title text in children."), *my_name);
	}

	if (nullptr == myText)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaUiPopup] '%s' found no content text in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaUiPopup] '%s' found the content text in children."), *my_name);
	}

	if (popupTitleDelegate.IsBound())
	{
		myTitle->SetText(popupTitleDelegate.Execute());
		myTitle->TextDelegate = popupTitleDelegate;
	}
	else
	{
		myTitle->SetText(popupTitle);
	}

	if (titleColorAndOpacityDelegate.IsBound())
	{
		myTitle->SetColorAndOpacity(titleColorAndOpacityDelegate.Execute());
		myTitle->ColorAndOpacityDelegate = titleColorAndOpacityDelegate;
	}
	else
	{
		myTitle->SetColorAndOpacity(titleColorAndOpacity);
	}

	if (popupContentTextDelegate.IsBound())
	{
		myText->SetText(popupContentTextDelegate.Execute());
		myText->TextDelegate = popupTitleDelegate;
	}
	else
	{
		myText->SetText(popupContentText);
	}

	if (textColorAndOpacityDelegate.IsBound())
	{
		myText->SetColorAndOpacity(textColorAndOpacityDelegate.Execute());
		myText->ColorAndOpacityDelegate = textColorAndOpacityDelegate;
	}
	else
	{
		myText->SetColorAndOpacity(textColorAndOpacity);
	}

	OnClosed.AddUniqueDynamic(this, &USagaUiPopup::HandleOnClosed);
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
