// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectComponents/ObstacleHPComponent.h"
#include "Obstacles/MapObstacle1.h"

// Sets default values for this component's properties
UObstacleHPComponent::UObstacleHPComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UObstacleHPComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UObstacleHPComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UObstacleHPComponent::TakeDamage(float DamageAmount)
{
	Health -= DamageAmount;
	CheckDeath();
}

float UObstacleHPComponent::GetCurrentHealth()
{
	return Health;
}

void UObstacleHPComponent::SetObjectHealth(float hp)
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
		AActor* ThisObstacleActor = GetOwner();
		if (ThisObstacleActor)
		{
			ThisObstacleActor->SetActorEnableCollision(false);
			ThisObstacleActor->FindComponentByClass<UPrimitiveComponent>()->SetSimulatePhysics(true);
			// Spawn the item box
			AMapObstacle1* Obstacle = Cast<AMapObstacle1>(ThisObstacleActor);
			if (Obstacle)
			{
				Obstacle->SpawnItemBox();
			}

			// Set Timer
			GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &UObstacleHPComponent::HandleDestruction, DestructionDelay);
		}
	}
}

void UObstacleHPComponent::HandleDestruction()
{
	// Call the function to remove the chaos destruction pieces
	RemoveDestructionPieces();
}

void UObstacleHPComponent::RemoveDestructionPieces()
{
	AActor* ThisObestacleActor = GetOwner();
	if (ThisObestacleActor)
	{
		ThisObestacleActor->Destroy();
	}
}
