#include "CharacterChoice/SagaSelectCharacter.h"
#include <UObject/ConstructorHelpers.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Animation/AnimInstance.h>

ASagaSelectCharacter::ASagaSelectCharacter()
	: Super()
	, mBody(), mMesh()
	//, humanCharacterAnimation()
	, mWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	mBody = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Body"));
	SetRootComponent(mBody);

	mBody->SetCollisionProfileName(TEXT("Player"));

	mMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/PlayerAssets/Player.Player'"));
	if (MeshAsset.Succeeded())
	{
		mMesh->SetSkeletalMesh(MeshAsset.Object);
	}

	mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mMesh->SetupAttachment(mBody);

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimAsset(TEXT("AnimBlueprint'/Game/PlayerAssets/Animation/AB_SagaPlayer.AB_SagaPlayer_C'"));
	if (AnimAsset.Succeeded())
	{
		humanCharacterAnimation = AnimAsset.Class;
	}

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));

	WeaponMesh->SetupAttachment(mMesh);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	if (mMesh)
	{
		WeaponMesh->SetupAttachment(mMesh, TEXT("c_middle1_r"));
	}
}

void
ASagaSelectCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//mMesh->SetAnimInstanceClass(humanCharacterAnimation.LoadSynchronous());
}

//Check needed after packaging
#if WITH_EDITOR
void ASagaSelectCharacter::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ASagaSelectCharacter, humanCharacterAnimation))
	{
		if (humanCharacterAnimation.IsValid())
		{
			UClass* AnimClass = humanCharacterAnimation.LoadSynchronous();
			if (AnimClass && AnimClass->IsChildOf(UAnimInstance::StaticClass()))
			{
				// 유효한 애니메이션 클래스
			}
			else
			{
				// 유효하지 않은 클래스 처리
				humanCharacterAnimation = nullptr;
			}
		}
	}
}
#endif