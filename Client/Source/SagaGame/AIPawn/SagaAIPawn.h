// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "EngineGlobals.h"
#include "GameFramework/Pawn.h"
#include "SagaAIMovementComponent.h"
#include "SagaAIPawn.generated.h"

UCLASS()
class SAGAGAME_API ASagaAIPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASagaAIPawn();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> mCapsule;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> mMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USagaAIMovementComponent> mMovement;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UArrowComponent> mArrowComponent;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
