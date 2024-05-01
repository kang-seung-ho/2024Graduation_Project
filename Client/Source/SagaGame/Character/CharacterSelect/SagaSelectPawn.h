// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//SelectController가 빙의할 Pawn 클래스
#include "../../SagaGameInfo.h"
#include "GameFramework/Pawn.h"
#include "SagaSelectPawn.generated.h"

UCLASS()
class SAGAGAME_API ASagaSelectPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASagaSelectPawn();

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* mRoot;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
