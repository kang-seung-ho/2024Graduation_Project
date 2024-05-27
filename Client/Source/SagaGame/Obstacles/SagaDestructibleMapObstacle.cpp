#include "Obstacles/SagaDestructibleMapObstacle.h"
#include "Components/SkeletalMeshComponent.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaDestructibleMapObstacle::ASagaDestructibleMapObstacle()
{
    PrimaryActorTick.bCanEverTick = true;

    // Initialize components
    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));


    static ConstructorHelpers::FObjectFinder<USkeletalMesh> ObstacleMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Obstacle/Pinata2.Pinata2'"));
    if (ObstacleMeshRef.Succeeded())
    {
        MeshComponent->SetSkeletalMesh(ObstacleMeshRef.Object);
    }

    static ConstructorHelpers::FObjectFinder<UAnimSequence> DeathAnimFinder(TEXT("AnimSequence'/Game/Terrains/Modelings/Pinata/Pinata_Anim_Bomb.Pinata_Anim_Bomb'"));
    if (DeathAnimFinder.Succeeded())
    {
        DeathAnimation = DeathAnimFinder.Object;
    }

    static ConstructorHelpers::FObjectFinder<UAnimSequence> HitAnimRef(TEXT("AnimSequence'/Game/Terrains/Modelings/Pinata/Pinata_Anim_Hit_01.Pinata_Anim_Hit_01'"));
    if (HitAnimRef.Succeeded())
    {
        HitAnimation = HitAnimRef.Object;
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

    UE_LOG(LogTemp, Warning, TEXT("Obstacle took %f damage, current health: %f"), DamageAmount, health);

	   if (health <= 0)
	   {
           

		   UE_LOG(LogTemp, Warning, TEXT("Obstacle is destroyed, spawning item..."));
		   FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 40.0f);
		   FRotator SpawnRotation = FRotator::ZeroRotator;
		   FActorSpawnParameters SpawnParameters;


		   if (DeathAnimation && MeshComponent)
		   {
			   MeshComponent->PlayAnimation(DeathAnimation, false);
		   }

           USagaNetworkSubSystem* NetworkSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USagaNetworkSubSystem>();
           if (NetworkSubsystem)
           {
               NetworkSubsystem->SetWhoWonByPinata(WhichTeam);
               GetWorldTimerManager().SetTimer(ChangeLevelTimerHandle, this, &ASagaDestructibleMapObstacle::OpenEndLevel, 3.0f, false);
           }


		   // Uncomment or modify this line as necessary to spawn an item
		   // GetWorld()->SpawnActor<ASagaItemBox>(ASagaItemBox::StaticClass(), SpawnLocation, SpawnRotation, SpawnParameters);
	   }
	   else {
		   if (HitAnimation && MeshComponent)
		   {
			   MeshComponent->PlayAnimation(HitAnimation, false);
		   }
	   }


    return DamageApplied;
}

void ASagaDestructibleMapObstacle::OpenEndLevel()
{
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("GameEndLevel"));
}