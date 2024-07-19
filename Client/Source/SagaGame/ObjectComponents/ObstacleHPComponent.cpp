#include "ObjectComponents/ObstacleHPComponent.h"
#include <UObject/Object.h>
#include <Templates/Casts.h>

#include "Obstacles/MapObstacle1.h"

UObstacleHPComponent::UObstacleHPComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called every frame
void UObstacleHPComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void
UObstacleHPComponent::TakeDamage(float DamageAmount)
{
	Health -= DamageAmount;
	CheckDeath();
}

float
UObstacleHPComponent::GetCurrentHealth()
const
{
	return Health;
}

void
UObstacleHPComponent::SetObjectHealth(float hp)
{
	Health = hp;
}

//void UObstacleHPComponent::CheckDeath()
//{
//	if (Health <= 0)
//	{
//		AActor* ThisObstacleActor = GetOwner();
//		if (ThisObstacleActor)
//		{
//			ThisObstacleActor->SetActorEnableCollision(false);
//			ThisObstacleActor->FindComponentByClass<UPrimitiveComponent>()->SetSimulatePhysics(true);
//
//			// TimerSet
//			GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &UObstacleHPComponent::HandleDestruction, DestructionDelay);
//		}
//	}
//}

void UObstacleHPComponent::CheckDeath()
{
	if (Health <= 0)
	{
		AActor* const owner = GetOwner();

		if (IsValid(owner))
		{
			owner->SetActorEnableCollision(false);
			owner->FindComponentByClass<UPrimitiveComponent>()->SetSimulatePhysics(true);

			/*
			Offline Mode:
			* Create a item box now.

			Online  Mode:
			* Defer the creation of an item box to send rpc.
			*/
			if (AMapObstacle1* Obstacle = Cast<AMapObstacle1>(owner))
			{
				Obstacle->SpawnItemBox();
			}

			GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &UObstacleHPComponent::HandleDestruction, DestructionDelay);
		}
	}
}

void
UObstacleHPComponent::HandleDestruction()
{
	// Call the function to remove the chaos destruction pieces
	RemoveDestructionPieces();
}

void
UObstacleHPComponent::RemoveDestructionPieces()
{
	AActor* const ThisObestacleActor = GetOwner();

	if (IsValid(ThisObestacleActor))
	{
		ThisObestacleActor->Destroy();
	}
}
