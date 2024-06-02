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

	virtual void Tick(float DeltaTime) override;

	void SetParticle(const FString& Path);
	void SetParticle(UParticleSystem* particle);

	void SetSound(const FString& Path);
	void SetSound(USoundBase* sound);

protected:
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* mParticle;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* mAudio;

	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnParticleFinish(UParticleSystemComponent* FinishComponent);
};
