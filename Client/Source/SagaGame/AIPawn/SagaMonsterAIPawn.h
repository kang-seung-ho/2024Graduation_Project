#pragma once
#include "CoreMinimal.h"
#include "AIPawn/SagaAIPawn.h"

#include "SagaMonsterAIPawn.generated.h"

UCLASS()
class SAGAGAME_API ASagaMonsterAIPawn : public ASagaAIPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASagaMonsterAIPawn();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
