// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SagaCharacterPlayer.h"
#include "SagaGummyBearPlayer.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API ASagaGummyBearPlayer : public ASagaCharacterPlayer
{
	GENERATED_BODY()
	
public:
	ASagaGummyBearPlayer();

protected:
	// ¿À¹ö·¦ ¹Ú½º
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* InteractionBox;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
