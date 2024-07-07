#pragma once
#include <CoreMinimal.h>
#include <UObject/Interface.h>
#include "../Item/SagaWeaponData.h"
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
	virtual void TakeItem(ESagaItemTypes Item) = 0;
};
