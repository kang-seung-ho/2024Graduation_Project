#include "SagaGummyBearPlayer.h"

ASagaGummyBearPlayer::ASagaGummyBearPlayer()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/NPCAssets/Modeling/Bear1.Bear1'"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}

	GetCapsuleComponent()->SetCapsuleSize(42.0f, 96.0f);
	SetRootComponent(GetCapsuleComponent());
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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


	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);
	InteractionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	InteractionBox->SetWorldScale3D(FVector(4.5f, 5.75f, 2.25f));
	InteractionBox->SetRelativeLocation(FVector(70.0f, 0.0f, 40.0f));


	// 오버랩 이벤트 바인드
	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &ASagaGummyBearPlayer::OnOverlapBegin);
	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &ASagaGummyBearPlayer::OnOverlapEnd);
}

void
ASagaGummyBearPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void
ASagaGummyBearPlayer::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
