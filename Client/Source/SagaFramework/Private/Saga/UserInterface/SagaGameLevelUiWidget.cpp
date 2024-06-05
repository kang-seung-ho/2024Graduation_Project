#include "Saga/Interface/SagaGameLevelUiWidget.h"
#include <UObject/Object.h>
#include <Kismet/KismetTextLibrary.h>
#include <Components/TextBlock.h>
#include <Templates/Casts.h>
#include <Blueprint/WidgetTree.h>

USagaGameLevelUiWidget::USagaGameLevelUiWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, myMinutesLabel(), mySecondsLabel()
	, myMinutes(), mySeconds()
{}

void
USagaGameLevelUiWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void
USagaGameLevelUiWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (const auto lbl = Cast<UTextBlock>(element))
			{
				const auto name = lbl->GetName();

				if (name == TEXT("MinutesText"))
				{
					myMinutesLabel = lbl;
				}
				else if (name == TEXT("SecondsText"))
				{
					mySecondsLabel = lbl;
				}
			}
		}
	);

	const auto my_name = GetName();
	if (IsValid(myMinutesLabel))
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaGameLevelUiWidget] '%s' found the minutes indicator label in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaGameLevelUiWidget] '%s' could not find the minutes indicator label in children."), *my_name);
	}
	if (IsValid(mySecondsLabel))
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaGameLevelUiWidget] '%s' found the seconds indicator label in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaGameLevelUiWidget] '%s' could not find the seconds indicator label in children."), *my_name);
	}
}

void
USagaGameLevelUiWidget::SetMinutes(const int32 minutes)
noexcept
{
	if (minutes != myMinutes and IsValid(myMinutesLabel))
	{
		myMinutes = minutes;

		const auto text = UKismetTextLibrary::Conv_IntToText(minutes, false, false, 2, 2);

		myMinutesLabel->SetText(text);
	}
}

void
USagaGameLevelUiWidget::SetSeconds(const int32 seconds)
noexcept
{
	if (seconds != mySeconds and IsValid(mySecondsLabel))
	{
		mySeconds = seconds;

		const auto text = UKismetTextLibrary::Conv_IntToText(seconds, false, false, 2, 2);

		mySecondsLabel->SetText(text);
	}
}
