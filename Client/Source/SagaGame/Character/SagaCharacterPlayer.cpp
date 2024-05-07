// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaCharacterPlayer.h"
#include "SagaPlayerAnimInstance.h"
#include "SagaGummyBearAnimInstance.h"
#include "../Item/SagaWeaponData.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

#include "SagaPlayableCharacter.h"
#include "SagaGummyBearPlayer.h"



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
	
}

void ASagaCharacterPlayer::SetTeamColorAndCollision()
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


void ASagaCharacterPlayer::BeginPlay()
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

void ASagaCharacterPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASagaCharacterPlayer::Attack()
{

}

void ASagaCharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isRunning)
	{
		ExecuteRun();
	}

	if (isForwardWalking || isStrafeWalking)
	{
		ExecuteWalk();
	}
	else
	{
		TerminateWalk();
	}

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


void ASagaCharacterPlayer::RotationCameraArm(float Scale)
{
	mArm->AddRelativeRotation(FRotator(Scale, 0.0, 0.0));

	FRotator	Rot = mArm->GetRelativeRotation();

	if (Rot.Pitch < -60.0)
		mArm->SetRelativeRotation(FRotator(-60.0, Rot.Yaw, Rot.Roll));

	else if (Rot.Pitch > 60.0)
		mArm->SetRelativeRotation(FRotator(60.0, Rot.Yaw, Rot.Roll));
}

void ASagaCharacterPlayer::ExecuteRotation(FVector2D RotationInput)
{
	// 2D 벡터를 3D 벡터로 변환하여 저장. Z 값은 0으로 설정
	const FVector InputValue = FVector(RotationInput.X, RotationInput.Y, 0.0f);


	AddControllerYawInput(InputValue.X);

	ASagaCharacterPlayer* ControlledPawn = this;
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->RotationCameraArm(InputValue.Y);
	}
}

void ASagaCharacterPlayer::TerminateRotation()
{
	
}

void ASagaCharacterPlayer::ExecuteJump()
{
	ASagaCharacterPlayer* ControlledPawn = this;
	if (ControlledPawn != nullptr)
	{
		if (ControlledPawn->CanJump())
		{
			ControlledPawn->Jump();
		}
	}
}

void ASagaCharacterPlayer::TerminateJump()
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

	walkDirection.Y = direction;
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

	walkDirection.X = direction;
}

void ASagaCharacterPlayer::ExecuteRun()
{
	ACharacter* MyCharacter = this;
	if (MyCharacter)
	{
		// 달리기 시작할 때 속도를 높임
		MyCharacter->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
}

void ASagaCharacterPlayer::TerminateRun()
{
	ACharacter* MyCharacter = this;
	if (MyCharacter)
	{
		// 달리기를 멈췄을 때 속도를 원래대로 복원
		MyCharacter->GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	}

}

void ASagaCharacterPlayer::ExecuteWalk()
{
	APawn* ControlledPawn = this;

	const FRotator Rotation = K2_GetActorRotation();
	const FRotator YawRotation = FRotator(0.0, Rotation.Yaw, 0.0);
	const FVector ForwardVector = YawRotation.Vector();
	const FVector RightVector = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);

	ControlledPawn->AddMovementInput(ForwardVector, walkDirection.Y);
	ControlledPawn->AddMovementInput(RightVector, walkDirection.X);

	auto move_dir = walkDirection.X * 90.f;
	if (walkDirection.Y > 0.f)
	{
		if (walkDirection.X < 0.f)
		{
			move_dir = -45.f;
		}
		else if (walkDirection.X > 0.f)
		{
			move_dir = 45.f;
		}
	}
	else if (walkDirection.Y < 0.f)
	{
		if (walkDirection.X < 0.f)
		{
			move_dir = -135.f;
		}
		else if (walkDirection.X > 0.f)
		{
			move_dir = 135.f;
		}
		else
		{
			move_dir = 180.f;
		}
	}
}

void ASagaCharacterPlayer::TerminateWalk()
{
	
}

void ASagaCharacterPlayer::ExecuteAttack()
{
	/*PlayAttackAnimation();*/
}

void ASagaCharacterPlayer::TerminateAttack()
{

}

void ASagaCharacterPlayer::ExecuteRide_Implementation()
{

}

void ASagaCharacterPlayer::TerminateRide_Implementation()
{

}
