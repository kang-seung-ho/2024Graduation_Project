#include "SagaSwordEffect.h"

ASagaSwordEffect::ASagaSwordEffect()
{
	PrimaryActorTick.bCanEverTick = true;

	mParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	mAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));

	SetRootComponent(mParticle);
	mAudio->SetupAttachment(mParticle);
}

void ASagaSwordEffect::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(mParticle->Template))
	{
		mParticle->OnSystemFinished.AddDynamic(this, &ASagaSwordEffect::OnParticleFinish);
	}
	
}

void ASagaSwordEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASagaSwordEffect::SetParticle(const FString& Path)
{
	UParticleSystem* Particle = LoadObject<UParticleSystem>(nullptr, *Path);
	if (Particle)
	{
		SetParticle(Particle);
	}
	
}

void ASagaSwordEffect::SetParticle(UParticleSystem* Particle)
{
	if (IsValid(Particle))
	{
		mParticle->SetTemplate(Particle);
		mParticle->OnSystemFinished.AddDynamic(this, &ASagaSwordEffect::OnParticleFinish);
	}
}

void ASagaSwordEffect::SetSound(const FString& Path)
{
	USoundBase* Sound = LoadObject<USoundBase>(nullptr, *Path);
	SetSound(Sound);
}

void ASagaSwordEffect::SetSound(USoundBase* sound)
{
	if (IsValid(sound))
	{
		mAudio->SetSound(sound);
		mAudio->Play();
	}
}

void ASagaSwordEffect::OnParticleFinish(UParticleSystemComponent* FinishComponent)
{
	Destroy();
}

