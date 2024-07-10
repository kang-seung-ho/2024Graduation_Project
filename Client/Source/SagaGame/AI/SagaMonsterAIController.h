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

protected:
	UPROPERTY(EditAnywhere)
	UAIPerceptionComponent* mAIPerception;
	
	UAISenseConfig_Sight* mSightConfig;

	UAISenseConfig_Damage* mDamageConfig;

	UPROPERTY(VisibleAnywhere)
	float mAISightRadius = 500.0f;

	UPROPERTY(VisibleAnywhere)
	float mLoseSightInRadius = 150.0f; //limit the inbound radius => Making the AI Pawn not get to the exact player's location

	UPROPERTY(VisibleAnywhere)
	float mAIFieldOfView = 120.0f; //AI's perceptible angle

	UPROPERTY(VisibleAnywhere)
	float mAISightAge = 1.f; //AI's sight age(duration)

	UPROPERTY(VisibleAnywhere)
	float mAISeenLocation = -1.f; //AI's seen location

	TObjectPtr<UBehaviorTree> mAITreeAsset;
	TObjectPtr<UBlackboardData> mBlackboardAsset;

protected:
	virtual void BeginPlay() override;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnTargetDetect(AActor* Target, FAIStimulus const Stimulus);

	UFUNCTION()
	void OnTargetForget(AActor* Target);
};
