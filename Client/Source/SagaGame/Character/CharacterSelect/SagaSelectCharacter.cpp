#include "Character/CharacterSelect/SagaSelectCharacter.h"
#include <UObject/ConstructorHelpers.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Animation/AnimInstance.h>

ASagaSelectCharacter::ASagaSelectCharacter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	mBody = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Body"));
	mMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));

	mBody->SetCollisionProfileName(TEXT("Player"));

	mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetRootComponent(mBody);

	mMesh->SetupAttachment(mBody);

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimAsset(TEXT("AnimBlueprint'/Game/PlayerAssets/Animation/AB_SagaPlayer.AB_SagaPlayer_C'"));
	if (AnimAsset.Succeeded())
	{
		humanCharacterAnimation = AnimAsset.Class;
	}


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

