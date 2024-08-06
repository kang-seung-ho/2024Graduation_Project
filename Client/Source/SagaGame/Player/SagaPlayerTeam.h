#pragma once
#include <CoreMinimal.h>

#include "SagaPlayerTeam.generated.h"

UENUM(BlueprintType, Category = "CandyLandSaga|Game|Player")
enum class [[nodiscard]] ESagaPlayerTeam : uint8
{
	Unknown,

	Red,
	Blue,

	Default = Red
};
