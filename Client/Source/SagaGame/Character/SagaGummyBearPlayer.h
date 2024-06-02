#pragma once
#include <CoreMinimal.h>

#include "Character/SagaCharacterPlayer.h"
#include "SagaGummyBearPlayer.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaGummyBearPlayer : public ASagaCharacterPlayer
{
	GENERATED_BODY()

public:
	ASagaGummyBearPlayer();

	virtual void Attack();

	virtual float ExecuteHurt(const float dmg) override;
	virtual void ExecuteDeath() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool isCanRide = false;

	UPROPERTY(EditAnywhere)
	int32 BearNum = 0;



protected:
	// 오버랩 박스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* InteractionBox;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void BeginPlay() override;
};
