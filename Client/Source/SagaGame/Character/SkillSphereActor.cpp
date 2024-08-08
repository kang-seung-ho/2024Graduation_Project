// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SkillSphereActor.h"

ASkillSphereActor::ASkillSphereActor()
{
	mRoot = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));

#if WITH_EDITOR

	mRoot->bVisualizeComponent = true;
#endif

	RootComponent = mRoot;

	mParticle->SetupAttachment(mRoot);

	SetRootComponent(mRoot);
}

void ASkillSphereActor::BeginPlay()
{
}

void ASkillSphereActor::Tick(float DeltaTime)
{
}

void ASkillSphereActor::SkillHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void ASkillSphereActor::SkillOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SkillOverlap"));
	FDamageEvent DamageEvent;
	OtherActor->TakeDamage(60.0f, DamageEvent, mOwnerController, this);
}

void ASkillSphereActor::SkillOverlapEnd(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ASkillSphereActor::DestroySkill()
{
	Destroy();
}
