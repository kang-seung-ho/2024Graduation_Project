// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SagaGameInfo.h"
#include <GameFramework/Actor.h>
#include "SagaSwordEffect.generated.h"

UCLASS()
class SAGAGAME_API ASagaSwordEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASagaSwordEffect();

protected:
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* mParticle;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* mAudio;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void SetParticle(const FString& Path);
	void SetParticle(UParticleSystem* particle);
	void SetSound(const FString& Path);
	void SetSound(USoundBase* sound);

public:
	UFUNCTION()
	void OnParticleFinish(UParticleSystemComponent* FinishComponent);
};
