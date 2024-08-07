// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SagaGame/Public/SagaGameInfo.h"
#include "Character/SkillActor.h"
#include "SkillSphereActor.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API ASkillSphereActor : public ASkillActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ASkillSphereActor();

public:
	virtual void SetSkillDistance(float Distance)
	{
		Super::SetSkillDistance(Distance);

		USphereComponent* Sphere = Cast<USphereComponent>(mRoot);

		if (IsValid(Sphere))
		{
			Sphere->SetSphereRadius(Distance);
		}
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual void SkillHit(AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	virtual void SkillOverlap(AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, const FHitResult& SweepResult);
	virtual void SkillOverlapEnd(AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void DestroySkill();
};
