#pragma once
#include "SagaGame.h"
#include "AI/SagaMonsterAIPawn.h"

#include "SagaGummyBearSmall.generated.h"

UCLASS()
class SAGAGAME_API ASagaGummyBearSmall : public ASagaMonsterAIPawn
{
	GENERATED_BODY()

public:
	ASagaGummyBearSmall();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	class UBoxComponent* CollisionBox;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void HandleExplosionAndDamage(AActor* OtherActor);

	// Explosion effect
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* ExplosionEffect;

	// Damage amount
	UPROPERTY(EditAnywhere, Category = "Damage")
	float DamageAmount;

	// Damage type class
	UPROPERTY(EditAnywhere, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;
};
