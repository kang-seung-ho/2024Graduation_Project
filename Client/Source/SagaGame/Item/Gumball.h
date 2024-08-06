// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../SagaGame/Public/SagaGameInfo.h"
#include "GameFramework/Actor.h"
#include "Gumball.generated.h"

UCLASS()
class SAGAGAME_API AGumball : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGumball();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
        class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

    void DestroyGumball();

    // Function to handle resetting the player's speed
    void ResetPlayerSpeed();

     FTimerHandle DestroyTimerHandle;

    FTimerHandle SpeedResetTimerHandle;

    class ASagaCharacterBase* OverlappingPlayer;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* BoxComponent;

};
