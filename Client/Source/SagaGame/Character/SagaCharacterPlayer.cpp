// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaCharacterPlayer.h"
#include "SagaPlayerAnimInstance.h"
#include "../Item/SagaWeaponData.h"

#include "Saga/Network/SagaNetworkSubSystem.h"


// Sets default values
ASagaCharacterPlayer::ASagaCharacterPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/PlayerAssets/Player.Player'"));
	if(MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}

	GetMesh()->SetRelativeLocation(FVector(0.0, 0.0, -88.0));
	GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	mArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	mArm->SetupAttachment(GetMesh());

	mCamera->SetupAttachment(mArm);

	mArm->SetRelativeLocation(FVector(0.0, 0.0, 150.0));
	mArm->SetRelativeRotation(FRotator(-15.0, 90.0, 0.0));

	mArm->TargetArmLength = 150.f;


	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
}

void ASagaCharacterPlayer::SetTeamColorAndCollision(int16 Team)
{
	teamColor = Team;
	if (teamColor == 1)
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Red"));
	}
	else if (teamColor == 2)
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Blue"));
	}
}


void ASagaCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	//이 함수 호출되기 전에 SkeletalMeshComponent에 지정된 AnimInstance 클래스 이용하여 사용하기 위한 객체 만들어놨음.
	mAnimInst = Cast<USagaPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	mWeapon = system->GetWeaponType();
	UE_LOG(LogTemp, Warning, TEXT("Weapon Type : %d"), (int)mWeapon);
}

void ASagaCharacterPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASagaCharacterPlayer::SwordAttack()
{

}

void ASagaCharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASagaCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ASagaCharacterPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return DamageAmount;
}

void ASagaCharacterPlayer::PlayAttackAnimation()
{
	mAnimInst->PlayAttackMontage();
}

void ASagaCharacterPlayer::RotationCameraArm(float Scale)
{
	mArm->AddRelativeRotation(FRotator(Scale, 0.0, 0.0));

	FRotator	Rot = mArm->GetRelativeRotation();

	if (Rot.Pitch < -60.0)
		mArm->SetRelativeRotation(FRotator(-60.0, Rot.Yaw, Rot.Roll));

	else if (Rot.Pitch > 60.0)
		mArm->SetRelativeRotation(FRotator(60.0, Rot.Yaw, Rot.Roll));
}

