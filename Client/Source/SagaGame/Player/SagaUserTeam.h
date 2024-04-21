#pragma once
#include "CoreMinimal.h"

#include "SagaUserTeam.generated.h"

UENUM(BlueprintType)
enum class [[nodiscard]] EUserTeam : uint8
{
	Unknown,
	Red, Blue
};
