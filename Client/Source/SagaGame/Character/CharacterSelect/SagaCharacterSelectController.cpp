// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaCharacterSelectController.h"

ASagaCharacterSelectController::ASagaCharacterSelectController()
{
	PrimaryActorTick.bCanEverTick = true;


}

void ASagaCharacterSelectController::BeginPlay()
{
	Super::BeginPlay();
}

void ASagaCharacterSelectController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult result;
	bool Hit = GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel5, false, result);

	if (Hit)
	{
		result.GetActor()->GetName();
	}
	else
	{

	}
}
