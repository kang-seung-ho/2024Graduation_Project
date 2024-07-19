#include "AI/SagaSpawnPoint.h"
#include <Components/SceneComponent.h>
#include <Components/ArrowComponent.h>

#include "AI/SagaAIPawn.h"

ASagaSpawnPoint::ASagaSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SetRootComponent(mRoot);

#if WITH_EDITORONLY_DATA
	mArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	mArrowComponent->SetupAttachment(mRoot);

	mRoot->bVisualizeComponent = true;
#endif
}

// Called when the game starts or when spawned
void ASagaSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	if (mSpawnType != ESpawnType::Loop)
	{
		Spawn();
	}
}

// Called every frame
void ASagaSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (mSpawnType)
	{
	case ESpawnType::Normal:
	{
		if (!IsValid(mSpawnActor))
		{
			mSpawnAccTime += DeltaTime;

			if (mSpawnAccTime >= mSpawnTime)
			{
				Spawn();
				mSpawnAccTime = 0.f;
			}
		}
	}
	break;

	case ESpawnType::Loop:
	{
		mSpawnAccTime += DeltaTime;

		if (mSpawnAccTime >= mSpawnTime)
		{
			Spawn();
			mSpawnAccTime -= mSpawnTime;

			if (mCount > 0)
			{
				--mCount;

				if (mCount == 0)
					Destroy();
			}
		}
	}
	break;
	}
}

void ASagaSpawnPoint::Spawn()
{
	if (!IsValid(mSpawnClass))
		return;

	FActorSpawnParameters	param;

	param.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector	Location = GetActorLocation();
	FRotator	Rot = GetActorRotation();

	ASagaAIPawn* AIPawn = Cast<ASagaAIPawn>(mSpawnClass->GetDefaultObject());

	if (IsValid(AIPawn))
	{
		float	HalfHeight = AIPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		Location.Z += HalfHeight;
	}

	mSpawnActor = GetWorld()->SpawnActor<AActor>(mSpawnClass, Location, Rot,
		param);

	AIPawn = Cast<ASagaAIPawn>(mSpawnActor);

	if (IsValid(AIPawn))
	{
		// ThisClass : it means the current Class. => SpawnPoint Class
		AIPawn->AddDeathDelegate(this, &ASagaSpawnPoint::DeathCallback);
	}

	//// �̵���θ� �߰��Ѵ�.
	//if (!mPatrolPointArray.IsEmpty())
	//{
	//	AIPawn->AddPatrolPoint(GetActorLocation());

	//	for (auto Point : mPatrolPointArray)
	//	{
	//		AIPawn->AddPatrolPoint(Point->GetActorLocation());
	//	}

	//	AIPawn->SetPatrolType(mPatrolType);
	//}

}


void ASagaSpawnPoint::DeathCallback()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Death"));

	mSpawnActor = nullptr;
}


