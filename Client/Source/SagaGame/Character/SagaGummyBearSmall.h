#pragma once
#include "SagaGame.h"
#include "AI/SagaMonsterAIPawn.h"

#include "SagaGummyBearSmall.generated.h"

UCLASS()
class SAGAGAME_API ASagaGummyBearSmall : public ASagaMonsterAIPawn
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Bear", meta = (ClampMin = "0", ClampMax = "10"))
	int32 myParentBearUniqueId;
	/* 작은 곰의 고유 번호

	*  서버, 클라 모두 같게 동기화됨
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Bear", meta = (ClampMin = "0", ClampMax = "10"))
	int32 miniBearUniqueId;

	ASagaGummyBearSmall();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetParentBearId(const int32 id) noexcept;
	
	UFUNCTION()
	void SetMiniBearId(const int32 id) noexcept;

	UFUNCTION(BlueprintPure)
	int32 GetParentBearId() const noexcept;
	
	UFUNCTION(BlueprintPure)
	int32 GetMiniBearId() const noexcept;

protected:
	virtual void BeginPlay() override;

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
