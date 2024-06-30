#pragma once
#include "SagaGame.h"
#include <AIController.h>
#include "SagaAIInfo.h"

#include "SagaMonsterAIController.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|AI")
class SAGAGAME_API ASagaMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASagaMonsterAIController();

	void RunAI();
	void StopAI();

protected:
	UPROPERTY()
	UAIPerceptionComponent* mAIPerception;
	
	class UAISenseConfig_Sight* mSightConfig;

	UPROPERTY(VisibleAnywhere)
	float mAISightRadius = 500.0f;

	UPROPERTY(VisibleAnywhere)
	float mLoseSightInRadius = 150.0f; //limit the inbound radius => Making the AI Pawn not get to the exact player's location

	UPROPERTY(VisibleAnywhere)
	float mAIFieldOfView = 360.0f; //AI's perceptible angle

	UPROPERTY(VisibleAnywhere)
	float mAISightAge = 5.0f; //AI's sight age(duration)

	UPROPERTY(VisibleAnywhere)
	float mAISeenLocation = 800.f; //AI's seen location

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	TObjectPtr<class UBlackboardData> BBAsset;

	UPROPERTY()
	TObjectPtr<class UBehaviorTree> BTAsset;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	void OnTargetDetect(AActor* Target, FAIStimulus const Stimulus);
};
