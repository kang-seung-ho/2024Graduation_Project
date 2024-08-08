#pragma once
#include "SagaGameInfo.h"
#include <GameFramework/Actor.h>

#include "SkillActor.generated.h"

UCLASS()
class SAGAGAME_API ASkillActor : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* mRoot;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* mParticle;

	float		mDistance = 0.f;
	AController* mOwnerController;
	APawn* mOwnerPawn;

public:
	ASkillActor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetSkillOwner(AController* Controller, APawn* Pawn)
	{
		mOwnerController = Controller;
		mOwnerPawn = Pawn;
	}

	UFUNCTION()
	virtual void SetSkillDistance(float Distance)
	{
		mDistance = Distance;
	}

public:
	void SetParticle(const FString& Path);
	void SetParticle(UParticleSystem* Particle);
	void SetSound(const FString& Path);
	void SetSound(USoundBase* Sound);

	UFUNCTION()
	virtual void SkillHit(AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	virtual void SkillOverlap(AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void SkillOverlapEnd(AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	virtual void BeginPlay() override;

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
