#pragma once
#include "SagaGame.h"
#include <GameFramework/Actor.h>
#include <Engine/TimerHandle.h>

#include "SagaDestructibleMapObstacle.generated.h"

UCLASS(Category = "CandyLandSaga|Game")
class SAGAGAME_API ASagaDestructibleMapObstacle : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY()
	FTimerHandle LevelChangeTimerHandle;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	class USkeletalMeshComponent* MeshComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//USkeletalMeshComponent* SKMeshComponent;

	UPROPERTY()
	float myHealth = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TeamPinataColor = 0; // 0: Red, 1: Blue

	UPROPERTY()
	UAnimSequence* DeathAnimation;

	UPROPERTY()
	UAnimSequence* HitAnimation;

	ASagaDestructibleMapObstacle();

	UFUNCTION()
	void DestroyObstacle();

	UFUNCTION()
	void ChangeLevel();

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	UPROPERTY()
	TSoftClassPtr<UAnimInstance> ObjectAnimation;

	UPROPERTY()
	class UNiagaraSystem* HitEffect;

	virtual void BeginPlay() override;
};
