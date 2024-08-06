#include "Saga/Interface/SagaUiShroud.h"
#include <Components/Widget.h>
#include <Layout/Geometry.h>

USagaUiShroud::USagaUiShroud(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, maxOpacity(0.5f), maxOpacityDelegate()
{
	myTickPolicy = ESagaLiveUserWidgetTickPolicy::TickBySelf;

	SetTransitionPeriod(ESagaLiveUserWidgetStates::Opening, 0.1f);
	SetTransitionPeriod(ESagaLiveUserWidgetStates::Closing, 1.0f);
}

void
USagaUiShroud::NativePreConstruct()
{
	Super::NativePreConstruct();

	SetRenderOpacity(maxOpacity);
}

void
USagaUiShroud::NativeConstruct()
{
	Super::NativeConstruct();

	SetRenderOpacity(maxOpacity * GetGraphicalProgress());
}

void
USagaUiShroud::NativeTick(const FGeometry& geometry, float delta_time)
{
	Super::NativeTick(geometry, delta_time);

	const auto progress = GetGraphicalProgress();

	SetRenderOpacity(maxOpacity * progress);
}

void
USagaUiShroud::SetMaxOpacity(const float value)
noexcept
{
	maxOpacity = value;
}

float
USagaUiShroud::GetMaxOpacity()
const noexcept
{
	return maxOpacity;
}
