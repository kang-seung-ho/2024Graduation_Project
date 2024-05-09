#pragma once
#include "CoreMinimal.h"

#include "SagaPlayerWeaponTypes.generated.h"

UENUM(BlueprintType)
enum class EPlayerWeapon : uint8
{
	LightSabor,
	WaterGun,
	Hammer
};
