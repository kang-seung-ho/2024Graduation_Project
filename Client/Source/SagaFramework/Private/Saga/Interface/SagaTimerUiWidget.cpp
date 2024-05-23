#include "Saga/Interface/SagaTimerUiWidget.h"
#include <Kismet/KismetTextLibrary.h>
#include <Components/TextBlock.h>
#include <Templates/Casts.h>
#include <Blueprint/WidgetTree.h>

USagaTimerUiWidget::USagaTimerUiWidget(const FObjectInitializer& initializer) noexcept
	: Super(initializer)
	, myMinutesLabel(), mySecondsLabel()
{}

void
USagaTimerUiWidget::SetMinutes(const int32 minutes)
noexcept
{
	if (nullptr != myMinutesLabel)
	{
		const auto text = UKismetTextLibrary::Conv_IntToText(minutes, false, false, 2, 2);
		myMinutesLabel->SetText(text);
	}
}

void
USagaTimerUiWidget::SetSeconds(const int32 seconds)
noexcept
{
	if (nullptr != mySecondsLabel)
	{
		const auto text = UKismetTextLibrary::Conv_IntToText(seconds, false, false, 2, 2);
		myMinutesLabel->SetText(text);
	}
}

void
USagaTimerUiWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (element == this) return;

			if (auto label = Cast<UTextBlock>(element))
			{
				const auto name = label->GetName();

				if (name == TEXT("Minute"))
				{
					myMinutesLabel = label;
				}
				else if (name == TEXT("Second"))
				{
					mySecondsLabel = label;
				}
			}
		}
	);

	const auto my_name = GetName();
	if (nullptr == myMinutesLabel)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaTimerUiWidget] '%s' could not find the minutes label in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaTimerUiWidget] '%s' found the minutes label in children."), *my_name);
	}

	if (nullptr == mySecondsLabel)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaTimerUiWidget] '%s' could not find the seconds label in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaTimerUiWidget] '%s' found the seconds label in children."), *my_name);
	}
}

void
USagaTimerUiWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}
