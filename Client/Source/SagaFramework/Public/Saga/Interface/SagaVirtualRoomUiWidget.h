#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"
#include <Blueprint/IUserObjectListEntry.h>

#include "SagaVirtualRoomUiWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Instanced Room Data UI Representer C++ Class"))
class SAGAFRAMEWORK_API USagaVirtualRoomUiWidget final : public USagaLiveUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
};
