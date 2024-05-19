#include "Saga/Interface/SagaUiShroud.h"
#include <Layout/Geometry.h>

USagaUiShroud::USagaUiShroud(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, maxOpacity(0.5f), maxOpacityDelegate()
{}

void
USagaUiShroud::NativePreConstruct()
{
	Super::NativePreConstruct();

	SetRenderOpacity(maxOpacity);
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
