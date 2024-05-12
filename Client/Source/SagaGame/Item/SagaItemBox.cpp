// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaItemBox.h"
#include "../Interface/SagaCharacterItemInterface.h"


// Sets default values
ASagaItemBox::ASagaItemBox()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

	RootComponent = Trigger;

	Mesh->SetupAttachment(Trigger);

	Effect->SetupAttachment(Trigger);

	Trigger->SetCollisionProfileName(CPROFILE_SAGATRIGGER);
	Trigger->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ASagaItemBox::OnOverlapBegin);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>BoxMeshRef(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (BoxMeshRef.Object)
	{
		Mesh->SetStaticMesh(BoxMeshRef.Object);
	}

	Mesh->SetRelativeLocation(FVector(0.f, -3.5f, -30.0f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	/*static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectRef(TEXT(""));
	if (EffectRef.Object)
	{
		Effect->SetTemplate(EffectRef.Object);
		Effect->bAutoActivate = false;
	}*/

	Trigger->SetCollisionProfileName(TEXT("Item"));

}

void ASagaItemBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Weapon == nullptr)
	{
		Destroy();
		return;
	}
	ISagaCharacterItemInterface* OverlappingPawn = Cast<ISagaCharacterItemInterface>(OtherActor);
	if (OverlappingPawn)
	{
		OverlappingPawn->TakeItem(Weapon);
	}
	Effect->Activate(true);
	Mesh->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	Effect->OnSystemFinished.AddDynamic(this, &ASagaItemBox::OnEffectFinished);
}

void ASagaItemBox::OnEffectFinished(UParticleSystemComponent* ParticleSystem)
{
	Destroy();
}

