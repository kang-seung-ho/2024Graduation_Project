#include "UI/SagaHpBarWidget.h"
#include <Components/ProgressBar.h>
#include <Templates/Casts.h>
#include <Misc/AssertionMacros.h>

#include "Interface/SagaCharacterWidgetInterface.h"

USagaHpBarWidget::USagaHpBarWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, MaxHp(-1)
{}

void
USagaHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PlayerHpBar")));
	ensure(nullptr != HpProgressBar);
}

void
USagaHpBarWidget::UpdateHpBar(float CurrentHp)
{
	ensure(MaxHp > 0.0f);

	if (nullptr != HpProgressBar)
	{
		const float NewPercent = CurrentHp / MaxHp;
		HpProgressBar->SetPercent(NewPercent);

		UE_LOG(LogSagaGame, Log, TEXT("CurrentHp: %f, MaxHp: %f, Percent: %f"), CurrentHp, MaxHp, NewPercent);
	}
}
