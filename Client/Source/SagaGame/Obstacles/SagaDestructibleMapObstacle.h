// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SagaGameInfo.h"
#include "GameFramework/Actor.h"
#include "SagaDestructibleMapObstacle.generated.h"

UCLASS()
class SAGAGAME_API ASagaDestructibleMapObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASagaDestructibleMapObstacle();

	UFUNCTION()
	void DestroyObstacle();

	UFUNCTION()
	void ChangeLevel();

	UPROPERTY()
	float health = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TeamPinataColor = 0; // 0: Red, 1: Blue

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TSoftClassPtr<UAnimInstance> ObjectAnimation;

	UPROPERTY()
	class UNiagaraSystem* HitEffect;

private:
	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY()
	FTimerHandle LevelChangeTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	USkeletalMeshComponent* MeshComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//USkeletalMeshComponent* SKMeshComponent;

	UPROPERTY()
	UAnimSequence* DeathAnimation;

	UPROPERTY()
	UAnimSequence* HitAnimation;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


};
