#include "AI/SagaMonsterAIController.h"

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

	mAIPerception->ConfigureSense(*mSightConfig);

	mDamageConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
	
	mDamageConfig->SetMaxAge(1.f);

	mAIPerception->ConfigureSense(*mDamageConfig);

	mAIPerception->SetDominantSense(mSightConfig->GetSenseImplementation());

	/*static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(TEXT("/Script/AIModule.BlackboardData'/Game/AI/BB_SagaSmallBear.BB_SagaSmallBear'"));
	if(nullptr != BBAssetRef.Object)
	{
		BBAsset = BBAssetRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetRef(TEXT("/Script/AIModule.BehaviorTree'/Game/AI/BT_SagaSmallBear.BT_SagaSmallBear'"));
	if(nullptr != BTAssetRef.Object)
	{
		BTAsset = BTAssetRef.Object;
	}*/
}

//void ASagaMonsterAIController::RunAI()
//{
//	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
//	if (UseBlackboard(BBAsset, BlackboardPtr))
//	{
//		bool RunResult = RunBehaviorTree(BTAsset);
//	}
//}
//
//void ASagaMonsterAIController::StopAI()
//{
//	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
//	if (BTComponent)
//	{
//		BTComponent->StopTree();
//	}
//}

void ASagaMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(IsValid(mAITreeAsset) && IsValid(mBlackboardAsset))
	{
		//Set the blackboard to use and use the data in the blackboard through the AI controller which has the BlackboardComponent.
		UBlackboardComponent* BlackboardRef = Blackboard;
		if(UseBlackboard(mBlackboardAsset, BlackboardRef))
		{
			//Run the behavior tree
			RunBehaviorTree(mAITreeAsset);
		}
	}
}

void ASagaMonsterAIController::OnUnPossess()
{
	Super::OnUnPossess();


}

void ASagaMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	mAIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetDetect);

	mAIPerception->OnTargetPerceptionForgotten.AddDynamic(this, &ThisClass::OnTargetForget);
}

void ASagaMonsterAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASagaMonsterAIController::OnTargetDetect(AActor* Target, FAIStimulus const Stimulus)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Target Detected"));
	
	if (Stimulus.WasSuccessfullySensed())
	{
		Blackboard->SetValueAsObject(TEXT("Target"), Target);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Target Successfully Sensed"));
	}
	else
	{
		Blackboard->SetValueAsObject(TEXT("Target"), nullptr);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Lost Target Sense"));
	}
}

void ASagaMonsterAIController::OnTargetForget(AActor* Target)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Target Forgotten"));
}
