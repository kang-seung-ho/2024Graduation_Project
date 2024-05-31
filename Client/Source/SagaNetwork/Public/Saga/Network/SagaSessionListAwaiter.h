#pragma once
#include <CoreMinimal.h>
#include "Kismet/BlueprintAsyncActionBase.h"

#include "SagaSessionListAwaiter.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Network")
class SAGANETWORK_API USagaRoomListAwaiter : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	USagaRoomListAwaiter();
};
