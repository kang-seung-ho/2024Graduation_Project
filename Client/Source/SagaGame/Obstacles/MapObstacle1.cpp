#include "Obstacles/MapObstacle1.h"
#include <Math/MathFwd.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/World.h>
#include <GameFramework/Controller.h>

#include "Item/SagaItemTypes.h"
#include "Item/SagaWeaponData.h"
#include "Item/SagaItemBox.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

AMapObstacle1::AMapObstacle1()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Initialize HPComponent
	HPComponent = CreateDefaultSubobject<UObstacleHPComponent>(TEXT("HPComponent"));

	// Set initial HP
	HPComponent->SetObjectHealth(50.0f);
}

void
AMapObstacle1::BeginPlay()
{
	Super::BeginPlay();

	if (HPComponent)
	{
		myHealth = HPComponent->GetCurrentHealth();
	}
}

void
AMapObstacle1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float
AMapObstacle1::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	myHealth -= DamageApplied;

	if (HPComponent)
	{
		HPComponent->TakeDamage(DamageApplied);
	}

	return DamageApplied;
}

void
AMapObstacle1::OnDestroy()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
		UE_LOG(LogSagaGame, Log, TEXT("[AMapObstacle1] Item Spawner %d is destroyed (Offline Mode)."), myItemId);

		(void)SpawnItemBox();
	}
	else
	{
		UE_LOG(LogSagaGame, Log, TEXT("[AMapObstacle1] Item Spawner %d is destroyed."), myItemId);

		if (0 < myItemId)
		{
			net->SendRpcPacket(ESagaRpcProtocol::RPC_DESTROY_ITEM_BOX, 0, myItemId);
		}
	}

	// Destroy this obstacle
	Destroy();
}

ASagaItemBox*
AMapObstacle1::SpawnItemBox()
const
{
	UE_LOG(LogSagaGame, Log, TEXT("[AMapObstacle1] Spawning an item with an id %d."), myItemId);

	const FVector loc = GetActorLocation() + FVector(0.0f, 0.0f, 40.0f);
	FActorSpawnParameters params{};

	ASagaItemBox* box = GetWorld()->SpawnActor<ASagaItemBox>(loc, FRotator::ZeroRotator, params);

	if (IsValid(box))
	{
		box->SetItemId(myItemId);

		if (UStaticMeshComponent* ItemMesh = box->GetMesh())
		{
			ItemMesh->SetSimulatePhysics(true);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			// Ensure it has a suitable collision profile
			//ItemMesh->SetCollisionProfileName(TEXT("Item"));

			// z value for the popping-up effect
			FVector LaunchVelocity = FVector(0.0f, 0.0f, 500.0f);
			ItemMesh->AddImpulse(LaunchVelocity, NAME_None, true);
		}
	}

	return box;
}
