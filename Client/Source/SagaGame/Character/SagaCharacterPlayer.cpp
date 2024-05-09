#include "SagaCharacterPlayer.h"
#include "SagaPlayerAnimInstance.h"
#include "SagaGummyBearAnimInstance.h"
#include "SagaPlayableCharacter.h"
#include "SagaGummyBearPlayer.h"
#include "../Item/SagaWeaponData.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

// Sets default values
ASagaCharacterPlayer::ASagaCharacterPlayer()
	: myId(-1)
	, walkDirection(), preferedDirection()
	, isForwardWalking(), isStrafeWalking()
	, isRunning()
	, wasMoved(), wasTilted()
	, tranformUpdateTimer()

{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/PlayerAssets/Player.Player'"));
	if (MeshAsset.Succeeded())
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
ASagaCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	//이 함수 호출되기 전에 SkeletalMeshComponent에 지정된 AnimInstance 클래스 이용하여 사용하기 위한 객체 만들어놨음.
	mAnimInst = Cast<USagaPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	mBearAnimInst = Cast<USagaGummyBearAnimInstance>(GetMesh()->GetAnimInstance());

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	mWeaponType = system->GetWeaponType();
	UE_LOG(LogTemp, Warning, TEXT("Weapon Type : %d"), (int)mWeaponType);

	system->GetLocalUserTeam(myTEAM);

	SetTeamColorAndCollision(); //NetworkSubsystem에서 받아온 팀 색깔로 설정
}

void
ASagaCharacterPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void
ASagaCharacterPlayer::Tick(float delta_time)
{
	Super::Tick(delta_time);

	// Accelerate toward to prefered direction
	if (preferedDirection != walkDirection)
	{
		// Acceleration delta
		constexpr double scalar_delta = 100;

		// Accelerate toward to the prefered direction
		auto dir_delta = preferedDirection - walkDirection;
		const auto dir_delta_size = dir_delta.Length();

		// Make it unit vector
		dir_delta.Normalize();

		walkDirection += dir_delta * FMath::Min(dir_delta_size, scalar_delta * delta_time);
	}

	// Do walk
	if (isForwardWalking || isStrafeWalking)
	{
		ExecuteWalk();
	}
}

// Called to bind functionality to input
void
ASagaCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void
ASagaCharacterPlayer::Attack()
{

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
	UE_LOG(LogTemp, Warning, TEXT("Entered PlayAttackAnimation"));

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	int32 CharacterMode = system->GetCurrentMode();
	UE_LOG(LogTemp, Warning, TEXT("Character Mode : %d"), CharacterMode);

	if (CharacterMode == 1)
	{
		if (mAnimInst != nullptr)
		{
			mAnimInst->PlayAttackMontage();
			UE_LOG(LogTemp, Warning, TEXT("This character is a SagaPlayableCharacter"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("mAnimInst is null"));
		}
	}
	else if (CharacterMode == 2)
	{
		if (mBearAnimInst != nullptr)
		{
			mBearAnimInst->PlayAttackMontage();
			UE_LOG(LogTemp, Warning, TEXT("This character is a SagaBear"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("mBearAnimInst is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Character Mode"));
	}
}

void
ASagaCharacterPlayer::ExecuteWalk()
{
	const FRotator rotation = K2_GetActorRotation();
	const FRotator yaw = FRotator(0.0, rotation.Yaw, 0.0);
	const FVector forward_dir = yaw.Vector();
	const FVector right_dir = FRotationMatrix(yaw).GetScaledAxis(EAxis::Y);

	AddMovementInput(forward_dir, walkDirection.Y);
	AddMovementInput(right_dir, walkDirection.X);
}

void
ASagaCharacterPlayer::TerminateStraightWalk()
{
	isForwardWalking = false;
	walkDirection.Y = 0;
}

void
ASagaCharacterPlayer::TerminateStrafeWalk()
{
	isStrafeWalking = false;
	walkDirection.X = 0;
}

void
ASagaCharacterPlayer::ExecuteRun()
{
	// 달리기 시작할 때 속도를 높임
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void
ASagaCharacterPlayer::TerminateRun()
{
	// 달리기를 멈췄을 때 속도를 원래대로 복원
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
}

void
ASagaCharacterPlayer::ExecuteJump_Implementation()
{
	if (CanJump())
	{
		Jump();
	}
}

void
ASagaCharacterPlayer::TerminateJump_Implementation()
{}

void
ASagaCharacterPlayer::ExecuteAttack_Implementation()
{
	/*PlayAttackAnimation();*/
}

void
ASagaCharacterPlayer::TerminateAttack_Implementation()
{

}

void
ASagaCharacterPlayer::ExecuteRide_Implementation()
{

}

void
ASagaCharacterPlayer::TerminateRide_Implementation()
{

}

void
ASagaCharacterPlayer::ProcessForwardWalk(const int& direction)
noexcept
{
	if (direction != 0)
	{
		isForwardWalking = true;
	}
	else
	{
		isForwardWalking = false;
	}

	preferedDirection.Y = static_cast<double>(direction);
}

void
ASagaCharacterPlayer::ProcessStrafeWalk(const int& direction)
noexcept
{
	if (direction != 0)
	{
		isStrafeWalking = true;
	}
	else
	{
		isStrafeWalking = false;
	}

	preferedDirection.X = static_cast<double>(direction);
}

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
