// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SagaSpawnPoint.h"
#include "AI/SagaAIPawn.h"

// Sets default values
ASagaSpawnPoint::ASagaSpawnPoint()
{
// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SetRootComponent(mRoot);

#if WITH_EDITORONLY_DATA
	mArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	mArrowComponent->SetupAttachment(mRoot);
#endif

	mRoot->bVisualizeComponent = true;
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
		if (!IsValid(mSpawnActor))
		{
			mSpawnAccTime += DeltaTime;

			if (mSpawnAccTime >= mSpawnTime)
			{
				Spawn();
				mSpawnAccTime = 0.f;
			}
		}
		break;
	case ESpawnType::Loop:
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
		AIPawn->AddDeathDelegate<ThisClass>(this, &ASagaSpawnPoint::DeathCallback);
	}

	//// 이동경로를 추가한다.
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
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Death"));

	mSpawnActor = nullptr;
}


