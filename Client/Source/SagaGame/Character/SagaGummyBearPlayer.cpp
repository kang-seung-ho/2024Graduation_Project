// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaGummyBearPlayer.h"

ASagaGummyBearPlayer::ASagaGummyBearPlayer()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/NPCAssets/Modeling/Bear1.Bear1'"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}

	GetMesh()->SetRelativeLocation(FVector(0.0, 0.0, -88.0));
	GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimAsset(TEXT("AnimBlueprint'/Game/NPCAssets/Animation/AB_GummyBear.AB_GummyBear_C'"));
	if (AnimAsset.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimAsset.Class);
	}

	mArm->SetRelativeLocation(FVector(0.0, 0.0, 250.0));
	mArm->SetRelativeRotation(FRotator(-30.0, 90.0, 0.0));

	mArm->TargetArmLength = 400.f;
}
