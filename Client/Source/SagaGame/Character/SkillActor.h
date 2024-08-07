// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SagaGame/Public/SagaGameInfo.h"
#include "GameFramework/Actor.h"
#include "SkillActor.generated.h"

UCLASS()
class SAGAGAME_API ASkillActor : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ASkillActor();

protected:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* mRoot;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* mParticle;

	float		mDistance = 0.f;
	AController* mOwnerController;
	APawn* mOwnerPawn;

public:
	void SetOwner(AController* Controller, APawn* Pawn)
	{
		mOwnerController = Controller;
		mOwnerPawn = Pawn;
	}

	virtual void SetSkillDistance(float Distance)
	{
		mDistance = Distance;
	}

public:
	void SetParticle(const FString& Path);
	void SetParticle(UParticleSystem* Particle);
	void SetSound(const FString& Path);
	void SetSound(USoundBase* Sound);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	virtual void SkillHit(AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	virtual void SkillOverlap(AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, const FHitResult& SweepResult);
	virtual void SkillOverlapEnd(AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void CollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnParticleFinish(UParticleSystemComponent* FinishComponent);

};
