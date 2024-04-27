#include "SagaPlayableCharacter.h"
#include "SagaPlayerAnimInstance.h"
// Fill out your copyright notice in the Description page of Project Settings.


ASagaPlayableCharacter::ASagaPlayableCharacter()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/PlayerAssets/Player.Player'"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}

	GetMesh()->SetRelativeLocation(FVector(0.0, 0.0, -88.0));
	GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimAsset(TEXT("AnimBlueprint'/Game/PlayerAssets/Animation/AB_SagaPlayer.AB_SagaPlayer_C'"));
	if (AnimAsset.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimAsset.Class);
	}
	


	/*mArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	mArm->SetupAttachment(GetMesh());

	mCamera->SetupAttachment(mArm);

	mArm->SetRelativeLocation(FVector(0.0, 0.0, 150.0));
	mArm->SetRelativeRotation(FRotator(-15.0, 90.0, 0.0));

	mArm->TargetArmLength = 150.f;*/

	//Weapon Add Action
	TakeWeaponAction.Add(FTakeWeaponDelegateWrapper(FOnTakeWeaponDelegate::CreateUObject(this, &ASagaPlayableCharacter::EquipHammer)));
	TakeWeaponAction.Add(FTakeWeaponDelegateWrapper(FOnTakeWeaponDelegate::CreateUObject(this, &ASagaPlayableCharacter::EquipLightSaber)));
	TakeWeaponAction.Add(FTakeWeaponDelegateWrapper(FOnTakeWeaponDelegate::CreateUObject(this, &ASagaPlayableCharacter::EquipWaterGun)));

	//Weapon
	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("c_middle1_r"));
}

void ASagaPlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASagaPlayableCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASagaPlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ASagaPlayableCharacter::EquipHammer(USagaWeaponData* WeaponData)
{
	USagaWeaponData* WeaponDataItem = Cast<USagaWeaponData>(WeaponData);
	if (WeaponDataItem)
	{
		Weapon->SetStaticMesh(WeaponDataItem->WeaponMesh);
	}

	mWeaponType = WeaponDataItem->WeaponType;
}

void ASagaPlayableCharacter::EquipLightSaber(USagaWeaponData* WeaponData)
{
	USagaWeaponData* WeaponDataItem = Cast<USagaWeaponData>(WeaponData);
	if (WeaponDataItem)
	{
		Weapon->SetStaticMesh(WeaponDataItem->WeaponMesh);
	}

	mWeaponType = WeaponDataItem->WeaponType;
}

void ASagaPlayableCharacter::EquipWaterGun(USagaWeaponData* WeaponData)
{
	USagaWeaponData* WeaponDataItem = Cast<USagaWeaponData>(WeaponData);
	if (WeaponDataItem)
	{
		Weapon->SetStaticMesh(WeaponDataItem->WeaponMesh);
	}
	mWeaponType = WeaponDataItem->WeaponType;
}

void ASagaPlayableCharacter::TakeItem(USagaWeaponData* WeaponData)
{
	if (WeaponData)
	{
		TakeWeaponAction[(uint8)WeaponData->WeaponType].WeaponDelegate.ExecuteIfBound(WeaponData);
	}
}

