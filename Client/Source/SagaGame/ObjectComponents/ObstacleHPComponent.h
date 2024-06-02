// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include "Components/ActorComponent.h"
#include "ObstacleHPComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SAGAGAME_API UObstacleHPComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UObstacleHPComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void TakeDamage(float DamageAmount);

	UFUNCTION()
	float GetCurrentHealth();

	UFUNCTION()
	void SetObjectHealth(float hp);



private:
	UPROPERTY(EditAnywhere, Category = Obstacles)
	float Health = 90.0f;

	UFUNCTION()
	void CheckDeath();

private:
	UPROPERTY(EditAnywhere)
	float DestructionDelay = 5.0f;

	UPROPERTY()
	FTimerHandle DestructionTimerHandle;


	UFUNCTION()
	void HandleDestruction();

	UFUNCTION()
	void RemoveDestructionPieces();
		
};
