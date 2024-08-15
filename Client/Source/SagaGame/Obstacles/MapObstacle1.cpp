#include "Obstacles/MapObstacle1.h"
#include <Math/MathFwd.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/World.h>
#include <GameFramework/Controller.h>

#include "Item/SagaItemTypes.h"
#include "Item/SagaItemBox.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

#include <NiagaraFunctionLibrary.h>
#include <NiagaraComponent.h>

AMapObstacle1::AMapObstacle1()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	HPComponent = CreateDefaultSubobject<UObstacleHPComponent>(TEXT("HPComponent"));

	HPComponent->SetObjectHealth(50.0f);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ObstacleNiagaraEffect(TEXT("/Script/Niagara.NiagaraSystem'/Game/VFX/VFX_Hit/NS_Hit.NS_Hit'"));
	if (ObstacleNiagaraEffect.Succeeded())
	{
		ObstacleHitEffect = ObstacleNiagaraEffect.Object;
		UE_LOG(LogTemp, Warning, TEXT("Obstacle HitEffect Loaded"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Obstacle HitEffect Not Loaded"));
	}
}

float
AMapObstacle1::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HPComponent)
	{
		HPComponent->TakeDamage(DamageApplied);
	}

	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();
	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ObstacleHitEffect, SpawnLocation, SpawnRotation);
	if (NiagaraComponent)
	{
		NiagaraComponent->SetAutoDestroy(true);
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

		// Destroy this obstacle
		Destroy();
	}
	else
	{
		if (0 <= myItemId)
		{
			UE_LOG(LogSagaGame, Log, TEXT("[AMapObstacle1] Item Spawner %d is destroyed."), myItemId);

			net->SendRpcPacket(ESagaRpcProtocol::RPC_DESTROY_ITEM_BOX, 0, myItemId);

			(void)SpawnItemBox();

			// Destroy this obstacle
			Destroy();
		}
		else
		{
			const auto name = GetName();

			UE_LOG(LogSagaGame, Warning, TEXT("[AMapObstacle1] Item Spawner %s has an invalid id."), *name);
		}
	}
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
