#pragma once
#include <CoreMinimal.h>

#include "SagaPlayerWeaponTypes.generated.h"

UENUM(BlueprintType, Category = "CandyLandSaga|Game|Player")
enum class EPlayerWeapon : uint8
{
	LightSabor,
	WaterGun,
	Hammer,
	None
};
