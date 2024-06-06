#include "Character/SagaGummyBearSmall.h"
#include "SagaGummyBearAnimInstance.h"

#include "AI/SagaMonsterAIController.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

// Sets default values
ASagaGummyBearSmall::ASagaGummyBearSmall()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/NPCAssets/Modeling/Bear.Bear'"));
	if (MeshAsset.Succeeded())
	{
		mMesh->SetSkeletalMesh(MeshAsset.Object);
	}

	mCapsule->SetCapsuleSize(50.0f, 35.0f);
	SetRootComponent(mCapsule);
	mCapsule->SetCollisionProfileName(TEXT("Enemy"));

	mMesh->SetRelativeLocation(FVector(0.0, 0.0, -45.0));
	mMesh->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));
	mMesh->SetWorldScale3D(FVector(0.3, 0.3, 0.3));
	mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimAsset(TEXT("AnimBlueprint'/Game/AI/AB_SmallBear.AB_SmallBear_C'"));
	if (AnimAsset.Succeeded())
	{
		mMesh->SetAnimInstanceClass(AnimAsset.Class);
	}

	AIControllerClass = ASagaMonsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void ASagaGummyBearSmall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASagaGummyBearSmall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
