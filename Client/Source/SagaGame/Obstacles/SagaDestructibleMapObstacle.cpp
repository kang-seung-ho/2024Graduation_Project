#include "Obstacles/SagaDestructibleMapObstacle.h"
#include "Components/SkeletalMeshComponent.h"

ASagaDestructibleMapObstacle::ASagaDestructibleMapObstacle()
{
    PrimaryActorTick.bCanEverTick = true;

    // Initialize components
    HealthComponent = CreateDefaultSubobject<UObstacleHPComponent>(TEXT("HealthComponent"));
    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));

    HealthComponent->SetObjectHealth(100.0f);

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> ObstacleMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Obstacle/Pinata2.Pinata2'"));
    if (ObstacleMeshRef.Succeeded())
    {
        MeshComponent->SetSkeletalMesh(ObstacleMeshRef.Object);
    }

    static ConstructorHelpers::FObjectFinder<UAnimSequence> DeathAnimFinder(TEXT("AnimSequence'/Game/Obstacle/Pinata2_Anim.Pinata2_Anim'"));
    if (DeathAnimFinder.Succeeded())
    {
        DeathAnimation = DeathAnimFinder.Object;
    }

    // Set up Mesh component as a Root Component
    RootComponent = MeshComponent;

    MeshComponent->SetCollisionProfileName(TEXT("Enemy")); // Collision Enabled for both Red and Blue Teams
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
}

void ASagaDestructibleMapObstacle::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ASagaDestructibleMapObstacle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

float ASagaDestructibleMapObstacle::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (HealthComponent)
    {
        HealthComponent->TakeDamage(DamageAmount);
        UE_LOG(LogTemp, Warning, TEXT("Obstacle took %f damage, current health: %f"), DamageAmount, HealthComponent->GetCurrentHealth())

        if (HealthComponent->GetCurrentHealth() <= 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("Obstacle is destroyed, spawning item..."));
            FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 40.0f);
            FRotator SpawnRotation = FRotator::ZeroRotator;
            FActorSpawnParameters SpawnParameters;


            if (DeathAnimation && MeshComponent)
            {
                MeshComponent->PlayAnimation(DeathAnimation, false);
            }


            // Uncomment or modify this line as necessary to spawn an item
            // GetWorld()->SpawnActor<ASagaItemBox>(ASagaItemBox::StaticClass(), SpawnLocation, SpawnRotation, SpawnParameters);
        }
    }

    return DamageApplied;
}
