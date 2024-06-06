#include "UI/SagaHpBarWidget.h"
#include <Misc/AssertionMacros.h>
#include <UObject/Object.h>
#include <Templates/Casts.h>
#include <Components/ProgressBar.h>

USagaHpBarWidget::USagaHpBarWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, myProgressBar()
	, MaxHp(-1)
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
USagaHpBarWidget::UpdateHpBar(float hp)
{
	ensure(MaxHp > 0.0f);

	if (IsValid(myProgressBar))
	{
		const float ratio = hp / MaxHp;
		myProgressBar->SetPercent(ratio);

		UE_LOG(LogSagaGame, Log, TEXT("[USagaHpBarWidget] Health: %f/%f (Percentage: %f)"), hp, MaxHp, ratio);
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("[USagaHpBarWidget] No progress bar."));
	}
}
