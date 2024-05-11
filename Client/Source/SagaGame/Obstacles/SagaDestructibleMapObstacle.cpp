#include "Obstacles/SagaDestructibleMapObstacle.h"


ASagaDestructibleMapObstacle::ASagaDestructibleMapObstacle()
{
 	PrimaryActorTick.bCanEverTick = true;

	// Initialize components
	HealthComponent = CreateDefaultSubobject<UObstacleHPComponent>(TEXT("HealthComponent"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ObstacleMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/Map/MapObstacles/Tree/cartoon_lowpoly_trees_fbx_Tree_2.cartoon_lowpoly_trees_fbx_Tree_2'"));
	if (ObstacleMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(ObstacleMeshRef.Object);
	}

	// Set up Mesh component as a Root Component
	RootComponent = MeshComponent;

	MeshComponent->SetCollisionProfileName(TEXT("Enemy")); //Collision Enabled for both Red and Blue Teams
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//SetActorScale3D(FVector(8.0f, 8.0f, 8.0f));
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

		if (HealthComponent->GetCurrentHealth() <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Obstacle is destroyed, spawning item..."));
			FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 40.0f); //맵 수정 시 높이 수정해줘야 함.
			FRotator SpawnRotation = FRotator::ZeroRotator;
			FActorSpawnParameters SpawnParameters;

			//GetWorld()->SpawnActor<ASagaItemBox>(ASagaItemBox::StaticClass(), SpawnLocation, SpawnRotation, SpawnParameters);

		}
	}

	return DamageApplied;
}

