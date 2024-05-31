#pragma once
#include "SagaNetwork.h"
#include <Engine/DeveloperSettings.h>
#include <Containers/UnrealString.h>

#include "Saga/Network/SagaNetworkConnectionCategory.h"
#include "SagaNetworkSettings.generated.h"

UCLASS(Config = Game, Category = "CandyLandSaga|Network")
class SAGANETWORK_API USagaNetworkSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "CandyLandSaga|Network")
	bool IsOfflineMode; // false

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "CandyLandSaga|Network")
	ESagaNetworkConnectionCategory ConnectionCategory; // Remote;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "CandyLandSaga|Network")
	FString RemoteAddress; // TEXT("61.84.90.208")

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "CandyLandSaga|Network")
	int32 RemotePort; // 40000

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "CandyLandSaga|Network")
	int32 LocalPort; // 40001
};
