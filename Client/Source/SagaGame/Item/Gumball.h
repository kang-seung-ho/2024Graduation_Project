#pragma once
#include "SagaGame.h"
#include <UObject/ObjectPtr.h>
#include <GameFramework/Actor.h>
#include <Engine/TimerHandle.h>

#include "Gumball.generated.h"

UCLASS()
class SAGAGAME_API AGumball : public AActor
{
	GENERATED_BODY()

public:
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxComponent;

	AGumball();

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY()
	TObjectPtr<class ASagaCharacterBase> OverlappingPlayer;

	UPROPERTY()
	FTimerHandle DestroyTimerHandle;

	UPROPERTY()
	FTimerHandle SpeedResetTimerHandle;

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
