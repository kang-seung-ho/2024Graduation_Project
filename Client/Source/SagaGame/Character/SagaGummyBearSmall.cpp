#include "Character/SagaGummyBearSmall.h"
#include "SagaGummyBearAnimInstance.h"

#include "AI/SagaMonsterAIController.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

#include "AI/SagaSmallBearAIController.h"
#include "AI/AIMonsterAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Character/SagaPlayableCharacter.h"

// Sets default values
ASagaGummyBearSmall::ASagaGummyBearSmall()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = ASagaSmallBearAIController::StaticClass();

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

	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetCollisionProfileName(TEXT("OverlapAll"));
	CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASagaGummyBearSmall::OnOverlapBegin);


	// Initialize explosion effect and damage values
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionEffectAsset(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (ExplosionEffectAsset.Succeeded())
	{
		ExplosionEffect = ExplosionEffectAsset.Object;
		UE_LOG(LogTemp, Warning, TEXT("Small Bear Effect Loaded"));
	}
	else
	{
		ExplosionEffect = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Small Bear Effect NOT Loaded"));
	}
	DamageAmount = 50.0f;
	DamageType = UDamageType::StaticClass();
}

// Called when the game starts or when spawned
void ASagaGummyBearSmall::BeginPlay()
{
	Super::BeginPlay();
	
	//mAnimInst = Cast<UAIMonsterAnimInstance>(mMesh->GetAnimInstance());
}

void
ASagaGummyBearSmall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void
ASagaGummyBearSmall::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherActor->IsA(ASagaPlayableCharacter::StaticClass()))
	{
		HandleExplosionAndDamage(OtherActor);
		Destroy();
	}
}

void
ASagaGummyBearSmall::HandleExplosionAndDamage(AActor* OtherActor)
{
	// Play explosion effect
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	// Apply damage to the other actor
	UGameplayStatics::ApplyDamage(OtherActor, DamageAmount, GetController(), this, nullptr); // nullptr for DamageType
}
