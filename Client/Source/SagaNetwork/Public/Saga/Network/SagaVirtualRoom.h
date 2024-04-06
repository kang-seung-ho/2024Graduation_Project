#pragma once
#include "Saga/Network/SagaVirtualSession.h"
#include "SagaVirtualRoom.generated.h"

USTRUCT(BlueprintType, Atomic, Category = "CandyLandSaga|Level|UI|Element")
struct SAGANETWORK_API FSagaVirtualRoom : public FSagaVirtualSession
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (NoResetToDefault, NoSpinbox = true, ClampMin = 0, UIMin = 0, ClampMax = 4, UIMax = 4))
	int32 MembersCount = 0;
};
