#include "AI/SagaMonsterAIController.h"
#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BlackboardData.h>
#include <BehaviorTree/BlackboardComponent.h>

#include "SagaGameInfo.h"

ASagaMonsterAIController::ASagaMonsterAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	mAIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	//register the AI perception
	SetPerceptionComponent(*mAIPerception); //using the dereference

	//adding object including options
	mSightConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	mSightConfig->SightRadius = mAISightRadius;
	mSightConfig->LoseSightRadius = mLoseSightInRadius + mAISightRadius;
	mSightConfig->PeripheralVisionAngleDegrees = mAIFieldOfView;
	mSightConfig->SetMaxAge(mAISightAge);
	mSightConfig->AutoSuccessRangeFromLastSeenLocation = mAISeenLocation;

	mSightConfig->DetectionByAffiliation.bDetectEnemies = true;
	mSightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	mSightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	mAIPerception->SetDominantSense(mSightConfig->GetSenseImplementation());
	mAIPerception->ConfigureSense(*mSightConfig);

	

	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(TEXT("/Script/AIModule.BlackboardData'/Game/AI/BB_SagaSmallBear.BB_SagaSmallBear'"));
	if(nullptr != BBAssetRef.Object)
	{
		BBAsset = BBAssetRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetRef(TEXT("/Script/AIModule.BehaviorTree'/Game/AI/BT_SagaSmallBear.BT_SagaSmallBear'"));
	if(nullptr != BTAssetRef.Object)
	{
		BTAsset = BTAssetRef.Object;
	}
}

void ASagaMonsterAIController::RunAI()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	if (UseBlackboard(BBAsset, BlackboardPtr))
	{
		bool RunResult = RunBehaviorTree(BTAsset);
	}
}

void ASagaMonsterAIController::StopAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		BTComponent->StopTree();
	}
}

void ASagaMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunAI();
}

void ASagaMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	mAIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetDetect);
}

void ASagaMonsterAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASagaMonsterAIController::OnTargetDetect(AActor* Target, FAIStimulus const Stimulus)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Target Detected"));
}