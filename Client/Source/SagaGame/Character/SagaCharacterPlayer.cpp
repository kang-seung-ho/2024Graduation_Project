#include "SagaCharacterPlayer.h"
#include "SagaPlayerAnimInstance.h"
#include "SagaGummyBearAnimInstance.h"
#include "../Item/SagaWeaponData.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaCharacterPlayer::ASagaCharacterPlayer()
	: myId(-1)
	, straightMoveDirection(), strafeMoveDirection()
	, isRunning()
	, myTEAM(EUserTeam::Unknown), mWeaponType(EPlayerWeapon::LightSabor)
	, animationMoveSpeed(), animationMoveAngle()
	, mAnimInst(nullptr), mBearAnimInst(nullptr)
	, mCamera(), mArm()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creating UCharacterMovementComponent
	bReplicates = true;
	SetReplicateMovement(true);

	const auto mesh = GetMesh();
	ensure(mesh != nullptr);

	mesh->SetRelativeLocation(FVector(0.0, 0.0, -88.0));
	mesh->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/PlayerAssets/Player.Player'"));
	if (MeshAsset.Succeeded())
	{
		mesh->SetSkeletalMesh(MeshAsset.Object);
	}

	mArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	mArm->SetupAttachment(mesh);
	mArm->SetRelativeLocation(FVector(0.0, 0.0, 150.0));
	mArm->SetRelativeRotation(FRotator(-15.0, 90.0, 0.0));
	mArm->TargetArmLength = 150.f;

	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	mCamera->SetupAttachment(mArm);
}

void
ASagaCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float
ASagaCharacterPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return DamageAmount;
}

void
ASagaCharacterPlayer::PlayAttackAnimation()
{
	UE_LOG(LogSagaGame, Warning, TEXT("Entered PlayAttackAnimation"));

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	int32 CharacterMode = system->GetCurrentMode();
	UE_LOG(LogSagaGame, Warning, TEXT("Character Mode : %d"), CharacterMode);

	if (CharacterMode == 1)
	{
		if (mAnimInst != nullptr)
		{
			mAnimInst->PlayAttackMontage();
			UE_LOG(LogSagaGame, Warning, TEXT("This character is a SagaPlayableCharacter"));
		}
		else
		{
			UE_LOG(LogSagaGame, Error, TEXT("mAnimInst is null"));
		}
	}
	else if (CharacterMode == 2)
	{
		if (mBearAnimInst != nullptr)
		{
			mBearAnimInst->PlayAttackMontage();
			UE_LOG(LogSagaGame, Warning, TEXT("This character is a SagaBear"));
		}
		else
		{
			UE_LOG(LogSagaGame, Error, TEXT("mBearAnimInst is null"));
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("Invalid Character Mode"));
	}
}

void
ASagaCharacterPlayer::SetTeamColorAndCollision()
{
	if (myTEAM == EUserTeam::Red)
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Red"));
	}
	else if (myTEAM == EUserTeam::Blue)
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Blue"));
	}
}

void
ASagaCharacterPlayer::SetTeamColorAndCollision(EUserTeam myTeam)
{
	if (myTEAM == EUserTeam::Red)
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Red"));
	}
	else if (myTEAM == EUserTeam::Blue)
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Blue"));
	}
}

void
ASagaCharacterPlayer::Attack()
{}

void
ASagaCharacterPlayer::RotationCameraArm(float Scale)
{
	mArm->AddRelativeRotation(FRotator(Scale, 0.0, 0.0));

	FRotator Rot = mArm->GetRelativeRotation();

	if (Rot.Pitch < -60.0)
		mArm->SetRelativeRotation(FRotator(-60.0, Rot.Yaw, Rot.Roll));

	else if (Rot.Pitch > 60.0)
		mArm->SetRelativeRotation(FRotator(60.0, Rot.Yaw, Rot.Roll));
}
