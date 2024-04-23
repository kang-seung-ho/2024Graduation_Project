// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../SagaGameInfo.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "SagaInGamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API ASagaInGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASagaInGamePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	float mMoveDir;

public:
	float GetMoveDir()
	{
		return mMoveDir;
	}

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	virtual void Tick(float DeltaTime) override;
	

protected:
	void OnMove(const FInputActionValue& Value);
	void OnAttack(const FInputActionValue& Value);
};
