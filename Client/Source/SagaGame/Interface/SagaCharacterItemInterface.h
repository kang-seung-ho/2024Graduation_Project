#pragma once
#include "SagaGame.h"
#include <UObject/Interface.h>

#include "Item/SagaItemTypes.h"

#include "SagaCharacterItemInterface.generated.h"

UINTERFACE(MinimalAPI)
class USagaCharacterItemInterface : public UInterface
{
	GENERATED_BODY()
};

class SAGAGAME_API ISagaCharacterItemInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void TakeItem(ESagaItemTypes item) = 0;
};
