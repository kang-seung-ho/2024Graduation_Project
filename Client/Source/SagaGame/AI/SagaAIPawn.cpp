#include "AI/SagaAIPawn.h"
#include "SagaGameInfo.h"

ASagaAIPawn::ASagaAIPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	mCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root"));
	mMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	mMovement = CreateDefaultSubobject<USagaAIMovementComponent>(TEXT("Movement"));

	SetRootComponent(mCapsule);

	mMovement->SetUpdatedComponent(mCapsule);

	mMesh->SetupAttachment(mCapsule);

#if WITH_EDITORONLY_DATA
	mArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	mArrowComponent->SetupAttachment(mCapsule);
#endif

	
	

}

void ASagaAIPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASagaAIPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ASagaAIPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return DamageAmount;
}
