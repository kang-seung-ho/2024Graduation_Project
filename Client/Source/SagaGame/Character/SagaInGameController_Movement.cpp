#include "SagaInGamePlayerController.h"
#include "Math/UnrealMathUtility.h"

double
ASagaInGamePlayerController::GetNormalizedMoveDir()
const noexcept
{
	const auto angle = FMath::RadiansToDegrees(FMath::Atan2(walkDirection.Y, walkDirection.X));

	return floor(angle / 45);
}
