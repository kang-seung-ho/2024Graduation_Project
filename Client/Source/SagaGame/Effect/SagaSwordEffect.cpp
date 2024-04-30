// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaSwordEffect.h"

// Sets default values
ASagaSwordEffect::ASagaSwordEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	mAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));

	//일반액터 상속받은 클래스-> 꼭 루트 컴포넌트 지정해줘야함
	SetRootComponent(mParticle);
	mAudio->SetupAttachment(mParticle);
}

// Called when the game starts or when spawned
void ASagaSwordEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASagaSwordEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASagaSwordEffect::SetParticle(const FString& Path)
{
}

void ASagaSwordEffect::SetParticle(UParticleSystem* particle)
{

}

