#include "Saga/Interface/SagaLiveUserWidgetStates.h"
#include <Math/UnrealMathUtility.h>

float
FSagaLiveUserWidgetStatus::Tick(const float& delta_time)
noexcept
{
	if (progressTime < progressPeriod)
	{
		progressTime = FMath::Clamp(progressTime, progressTime + delta_time, progressPeriod);
	}

	return progressTime / progressPeriod;
}

void
FSagaLiveUserWidgetStatus::SetPeriod(const float value)
noexcept
{
	ensure(0 < value);

	progressPeriod = value;
}

void
FSagaLiveUserWidgetStatus::SetProgress(const float value)
noexcept
{
	progressTime = FMath::Lerp(0, progressPeriod, value);
}

float
FSagaLiveUserWidgetStatus::GetProgress()
const noexcept
{
	return progressTime / progressPeriod;
}
