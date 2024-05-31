#include "SagaCharacterSelectWidget.h"
#include <UObject/UObjectGlobals.h>
#include <Templates/Casts.h>
#include <Blueprint/WidgetTree.h>

#include "Saga/Interface/SagaTimerUiWidget.h"
#include "Saga/Interface/SagaUiButton.h"

USagaCharacterSelectWidget::USagaCharacterSelectWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, myTimer(), myStartButton()
{}

void
USagaCharacterSelectWidget::StartButtonEnable(bool flag)
{
	if (nullptr != myStartButton)
	{
		myStartButton->SetUserInteractable(flag);
	}
}

void
USagaCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (element == this) return;

			if (auto timer = Cast<USagaTimerUiWidget>(element))
			{
				myTimer = timer;
			}
			else if (auto btn = Cast<USagaUiButton>(element))
			{
				myStartButton = btn;
			}
		}
	);

	const auto my_name = GetName();
	if (nullptr == myTimer)
	{
		UE_LOG(LogSagaGame, Fatal, TEXT("[USagaCharacterSelectWidget] '%s' could not found the timer in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaGame, Log, TEXT("[USagaCharacterSelectWidget] '%s' found the timer in children."), *my_name);
	}

	if (nullptr == myStartButton)
	{
		UE_LOG(LogSagaGame, Fatal, TEXT("[USagaCharacterSelectWidget] '%s' could not found the start button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaGame, Log, TEXT("[USagaCharacterSelectWidget] '%s' found the start button in children."), *my_name);
	}
}

void
USagaCharacterSelectWidget::NativeTick(const FGeometry& geometry, float delta_time)
{
	Super::NativeTick(geometry, delta_time);
}
