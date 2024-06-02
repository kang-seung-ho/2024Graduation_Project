#pragma once
#include <CoreMinimal.h>
#include <UObject/Interface.h>

#include "SagaCharacterWidgetInterface.generated.h"

UINTERFACE(MinimalAPI)
class USagaCharacterWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class SAGAGAME_API ISagaCharacterWidgetInterface
{
	GENERATED_BODY()

public:
	virtual void SetupCharacterWidget(class USagaUserWidget* InUserWidget) = 0;
};
