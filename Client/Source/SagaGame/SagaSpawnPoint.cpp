// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaSpawnPoint.h"
#include "AIPawn/SagaAIPawn.h"

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

	if(mSpawnType != ESpawnType::Loop)
		Spawn();
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

			if(mSpawnAccTime >= mSpawnTime)
			{
				Spawn();
				mSpawnAccTime = 0.0f;
			}
		}
		break;

	case ESpawnType::Loop:
		mSpawnAccTime += DeltaTime;

		if (mSpawnAccTime >= mSpawnTime)
		{
			Spawn();
			mSpawnAccTime -= mSpawnTime;

			//무한으로 동작해햐할 경우
			if (mCount > 0)
			{
				--mCount;

				if (mCount == 0)
					Destroy();
			}
		}
		break;

	case ESpawnType::Once:
		break;
	}

}

void ASagaSpawnPoint::Spawn()
{
	if (!IsValid(mSpawnClass))
		return;

	FActorSpawnParameters param;

	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector Location = GetActorLocation();
	FRotator Rot = GetActorRotation();

	ASagaAIPawn* AIPawn = Cast<ASagaAIPawn>(mSpawnClass->GetDefaultObject<ASagaAIPawn>());
	if (IsValid(AIPawn)) //AI일 경우의 위치 조정
	{
		float HalfHeight = AIPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		Location.Z += HalfHeight;
	}

	mSpawnActor = GetWorld()->SpawnActor<AActor>(mSpawnClass, Location, Rot, param);
}

