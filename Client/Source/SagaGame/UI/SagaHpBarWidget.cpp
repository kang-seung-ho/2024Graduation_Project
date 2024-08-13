#include "UI/SagaHpBarWidget.h"
#include <Misc/AssertionMacros.h>
#include <UObject/Object.h>
#include <Templates/Casts.h>
#include <Components/ProgressBar.h>

USagaHpBarWidget::USagaHpBarWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, myProgressBar()
{}

void
USagaHpBarWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	const auto progressbar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PlayerHpBar")));

	if (IsValid(progressbar))
	{
		progressbar->SetPercent(1);
	}
}

void
USagaHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	myProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PlayerHpBar")));
	ensure(nullptr != myProgressBar);
}

void
USagaHpBarWidget::UpdateHpBar(const float percentage)
{
	if (IsValid(myProgressBar))
	{
		myProgressBar->SetPercent(percentage);
		myProgressBar->SynchronizeProperties();

		UE_LOG(LogSagaGame, Log, TEXT("[USagaHpBarWidget] Health percentage: %f"), percentage);
	}
}
