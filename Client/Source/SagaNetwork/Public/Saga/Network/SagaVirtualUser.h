#pragma once
#include "Saga/Network/SagaVirtualSession.h"

#include "SagaVirtualUser.generated.h"

enum class EUserTeam : uint8;

USTRUCT(BlueprintType, Atomic, Category = "CandyLandSaga|Level|UI|Element")
struct SAGANETWORK_API FSagaVirtualUser : public FSagaVirtualSession
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Network|Session")
	TObjectPtr<class ASagaInGamePlayerController> localController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Network|Session")
	TObjectPtr<class ASagaRemotePlayerCharacter> remoteCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "CandyLandSaga|Network|Session")
	EUserTeam myTeam;
};
