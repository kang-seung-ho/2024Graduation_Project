// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPawn/SagaBearPawn.h"
#include "Components/CapsuleComponent.h"


ASagaBearPawn::ASagaBearPawn()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT(""));

	if (MeshAsset.Succeeded())
	{
		mMesh->SetSkeletalMesh(MeshAsset.Object);
	}

	mCapsule->SetCapsuleHalfHeight(85.f);
	mCapsule->SetCapsuleRadius(30.f);

	mMesh->SetRelativeLocation(FVector(0.f, 0.f, -85.f));
}

void ASagaBearPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ASagaBearPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
