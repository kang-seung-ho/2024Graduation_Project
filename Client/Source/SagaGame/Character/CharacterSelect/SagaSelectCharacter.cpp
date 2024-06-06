#include "Character/CharacterSelect/SagaSelectCharacter.h"
#include <UObject/ConstructorHelpers.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Animation/AnimInstance.h>

ASagaSelectCharacter::ASagaSelectCharacter()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	mBody = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Body"));
	SetRootComponent(mBody);

	mBody->SetCollisionProfileName(TEXT("Player"));

	mMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	mMesh->SetupAttachment(mBody);

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimAsset(TEXT("AnimBlueprint'/Game/PlayerAssets/Animation/AB_SagaPlayer.AB_SagaPlayer_C'"));
	if (AnimAsset.Succeeded())
	{
		humanCharacterAnimation = AnimAsset.Class;
	}
}

void
ASagaSelectCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	mMesh->SetAnimInstanceClass(humanCharacterAnimation.LoadSynchronous());
}
