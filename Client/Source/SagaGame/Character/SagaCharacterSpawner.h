#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SagaGame/Player/SagaUserTeam.h"
#include "SagaCharacterSpawner.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game")
class SAGAGAME_API ASagaCharacterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASagaCharacterSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	TArray<int32> awaitingPlayers;
};
