#pragma once
#include "SagaGame.h"

#include "SagaItemTypes.generated.h"

UENUM(BlueprintType, Category = "CandyLandSaga|Game|Item")
enum class ESagaItemTypes : uint8
{
	Drink,
	Gum,
	SmokeBomb
};
