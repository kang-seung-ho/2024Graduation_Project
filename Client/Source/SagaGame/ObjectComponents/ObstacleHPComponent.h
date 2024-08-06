// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SagaGame.h"
#include <Components/ActorComponent.h>

#include "ObstacleHPComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SAGAGAME_API UObstacleHPComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObstacleHPComponent();

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void TakeDamage(float DamageAmount);

	UFUNCTION()
	void SetObjectHealth(float hp);

	UFUNCTION()
	float GetCurrentHealth() const;

private:
	UPROPERTY(EditAnywhere, Category = Obstacles)
	float Health = 90.0f;

	UPROPERTY(EditAnywhere)
	float DestructionDelay = 5.0f;

	UPROPERTY()
	FTimerHandle DestructionTimerHandle;

	UFUNCTION()
	void CheckDeath();

private:
	UFUNCTION()
	void HandleDestruction();

	UFUNCTION()
	void RemoveDestructionPieces();
};
