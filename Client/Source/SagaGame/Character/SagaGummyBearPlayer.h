#pragma once
#include "CoreMinimal.h"

#include "Character/SagaCharacterPlayer.h"
#include "SagaGummyBearPlayer.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaGummyBearPlayer : public ASagaCharacterPlayer
{
	GENERATED_BODY()
	
public:
	ASagaGummyBearPlayer();

	virtual void Attack();

protected:
	// 오버랩 박스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* InteractionBox;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void BeginPlay() override;

	virtual float TakeDamage(const float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	UPROPERTY()
	float BearHp = 200.0f;
};
