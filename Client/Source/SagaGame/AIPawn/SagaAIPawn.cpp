#include "AIPawn/SagaAIPawn.h"
#include "Components/ArrowComponent.h"

// Sets default values
ASagaAIPawn::ASagaAIPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

// Called when the game starts or when spawned
void ASagaAIPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASagaAIPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}