#pragma once
#include "SagaGame.h"
#include <GameFramework/Actor.h>

#include "Gumball.generated.h"

UCLASS()
class SAGAGAME_API AGumball : public AActor
{
	GENERATED_BODY()

public:
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComponent;

	AGumball();

	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY()
	FTimerHandle DestroyTimerHandle;

	UPROPERTY()
	FTimerHandle SpeedResetTimerHandle;

	UPROPERTY()
	class ASagaCharacterBase* OverlappingPlayer;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void DestroyGumball();

	// Function to handle resetting the player's speed
	UFUNCTION()
	void ResetPlayerSpeed();
};
