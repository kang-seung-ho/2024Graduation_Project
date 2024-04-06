#pragma once
#include "CoreMinimal.h"

#include "UserTeam.generated.h"

UENUM(BlueprintType)
enum class [[nodiscard]] EUserTeam : uint8
{
	Unknown,
	Red, Blue
};
