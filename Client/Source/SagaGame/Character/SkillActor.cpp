// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SkillActor.h"

ASkillActor::ASkillActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
}

void ASkillActor::SetParticle(const FString& Path)
{
	// Can use Function that is not constructor.
	// *Path has to be const TCHAR* type. So, we need to dereference FString to get TCHAR array's start address.
	UParticleSystem* Particle = LoadObject<UParticleSystem>(nullptr, *Path);

	SetParticle(Particle);
}

void ASkillActor::SetParticle(UParticleSystem* Particle)
{
	if (IsValid(Particle))
	{
		mParticle->SetTemplate(Particle);
		mParticle->OnSystemFinished.AddDynamic(this,
			&ASkillActor::OnParticleFinish);
	}
}

void ASkillActor::SetSound(const FString& Path)
{
	USoundBase* Sound = LoadObject<USoundBase>(nullptr, *Path);

	SetSound(Sound);
}

void ASkillActor::SetSound(USoundBase* Sound)
{
	if (IsValid(Sound))
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation());
	}
}

void ASkillActor::BeginPlay()
{
	Super::BeginPlay();

	mRoot->OnComponentHit.AddDynamic(this, &ASkillActor::CollisionHit);
	mRoot->OnComponentBeginOverlap.AddDynamic(this, &ASkillActor::BeginOverlap);
	mRoot->OnComponentEndOverlap.AddDynamic(this, &ASkillActor::EndOverlap);
}

void ASkillActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASkillActor::SkillHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void ASkillActor::SkillOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, const FHitResult& SweepResult)
{
}

void ASkillActor::SkillOverlapEnd(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ASkillActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	SkillOverlap(OtherActor, OtherComp, OtherBodyIndex, SweepResult);
}

void ASkillActor::EndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	SkillOverlapEnd(OtherActor, OtherComp, OtherBodyIndex);
}

void ASkillActor::CollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	SkillHit(OtherActor, OtherComp, NormalImpulse, Hit);
}

void ASkillActor::OnParticleFinish(UParticleSystemComponent* FinishComponent)
{
	Destroy();
}

