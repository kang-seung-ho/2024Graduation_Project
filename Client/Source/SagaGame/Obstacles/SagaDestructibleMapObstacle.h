// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../SagaGameInfo.h"
#include "GameFramework/Actor.h"
#include "ObjectComponents/ObstacleHPComponent.h"
#include "SagaDestructibleMapObstacle.generated.h"

UCLASS()
class SAGAGAME_API ASagaDestructibleMapObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASagaDestructibleMapObstacle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TSoftClassPtr<UAnimInstance> ObjectAnimation;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	UObstacleHPComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	USkeletalMeshComponent* MeshComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//USkeletalMeshComponent* SKMeshComponent;

	UPROPERTY()
	UAnimSequence* DeathAnimation;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


};
