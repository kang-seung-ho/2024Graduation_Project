// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterSelect/SagaSelectCharacter.h"

// Sets default values
ASagaSelectCharacter::ASagaSelectCharacter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	mBody = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Body"));
	mMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));

	mBody->SetCollisionProfileName(TEXT("Player"));

	mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RootComponent = mBody;

	mMesh->SetupAttachment(mBody);

}

// Called when the game starts or when spawned
void ASagaSelectCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASagaSelectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

