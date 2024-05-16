#pragma once
#include <CoreMinimal.h>
#include <Components/WidgetComponent.h>

#include "SagaWidgetComponent.generated.h"

UCLASS()
class SAGAGAME_API USagaWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

protected:
	virtual void InitWidget() override;
	
};
