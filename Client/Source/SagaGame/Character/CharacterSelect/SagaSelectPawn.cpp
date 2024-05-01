// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaSelectPawn.h"

// Sets default values
ASagaSelectPawn::ASagaSelectPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	SetRootComponent(mRoot);

	mCamera->SetupAttachment(mRoot);

}

// Called when the game starts or when spawned
void ASagaSelectPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASagaSelectPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASagaSelectPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

