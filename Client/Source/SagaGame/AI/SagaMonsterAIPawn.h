#pragma once
#include "SagaGame.h"
#include "AI/SagaAIPawn.h"

#include "SagaMonsterAIPawn.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|AI")
class SAGAGAME_API ASagaMonsterAIPawn : public ASagaAIPawn
{
	GENERATED_BODY()

public:
	ASagaMonsterAIPawn();

protected:
	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* mAIPerception; //This is Actor component type

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	
};