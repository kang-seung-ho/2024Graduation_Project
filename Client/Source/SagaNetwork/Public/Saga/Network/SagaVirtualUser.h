#pragma once
#include "Saga/Network/SagaVirtualSession.h"
#include "SagaGame/Player/SagaUserTeam.h"
#include "SagaGame/Player/SagaPlayerWeaponTypes.h"

#include "SagaVirtualUser.generated.h"

USTRUCT(BlueprintType, Atomic, Category = "CandyLandSaga|Level|UI|Element")
struct SAGANETWORK_API FSagaVirtualUser : public FSagaVirtualSession
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Network|Session")
	TObjectPtr<class ASagaInGamePlayerController> localController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Network|Session")
	TObjectPtr<class ASagaCharacterPlayer> remoteCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "CandyLandSaga|Network|Session")
	EUserTeam myTeam;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "CandyLandSaga|Network|Session")
	EPlayerWeapon myWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Network|Session")
	float myHealth;
};
