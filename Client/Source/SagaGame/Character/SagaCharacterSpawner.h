#pragma once
#include <CoreMinimal.h>
#include <GameFramework/Actor.h>

#include "SagaGame/Player/SagaUserTeam.h"
#include "SagaCharacterSpawner.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game")
class SAGAGAME_API ASagaCharacterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ASagaCharacterSpawner();

	UFUNCTION()
	FVector GetInitialSpawnPoint();

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	int32 spawnCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	TArray<int32> awaitingPlayers;
};
