#include "AIPawn/SagaBearPawn.h"

ASagaBearPawn::ASagaBearPawn()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("")); //레퍼런스 복사

	if (MeshAsset.Succeeded())
	{
		mMesh->SetSkeletalMesh(MeshAsset.Object);
	}

	mCapsule->SetCapsuleHalfHeight(85.f);
	mCapsule->SetCapsuleRadius(30.f);

	mMesh->SetRelativeLocation(FVector(0.f, 0.f, -85.f));
	mMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimAsset(TEXT("")); //레퍼런스 복사후 끝에 _C 붙이기
	if(AnimAsset.Succeeded())
	{
		mMesh->SetAnimInstanceClass(AnimAsset.Class);
	}
}

void ASagaBearPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ASagaBearPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
