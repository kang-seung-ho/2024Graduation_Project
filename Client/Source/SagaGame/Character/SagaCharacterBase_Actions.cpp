#include "Character/SagaCharacterBase.h"
#include <Engine/EngineTypes.h>
#include <UObject/Object.h>
#include <GameFramework/CharacterMovementComponent.h>

#include "Player/SagaPlayerTeam.h"
#include "UI/SagaWidgetComponent.h"

void
ASagaCharacterBase::ExecuteStraightWalk(const int& direction)
noexcept
{
	//UE_LOG(LogSagaGame, Log, TEXT("[Character] ExecuteStraightWalk (%d)"), direction);

	straightMoveDirection = direction;
}

void
ASagaCharacterBase::ExecuteStrafeWalk(const int& direction)
noexcept
{
	//UE_LOG(LogSagaGame, Log, TEXT("[Character] ExecuteStrafeWalk (%d)"), direction);

	strafeMoveDirection = direction;
}

void
ASagaCharacterBase::TerminateStraightWalk()
{
	straightMoveDirection = 0;
}

void
ASagaCharacterBase::TerminateStrafeWalk()
{
	strafeMoveDirection = 0;
}

void
ASagaCharacterBase::ExecuteRun()
{
	// 달리기 시작할 때 속도를 높임
	GetCharacterMovement()->MaxWalkSpeed = GetMaxMoveSpeed(true);
}

void
ASagaCharacterBase::TerminateRun()
{
	// 달리기를 멈췄을 때 속도를 원래대로 복원
	GetCharacterMovement()->MaxWalkSpeed = GetMaxMoveSpeed(false);
}

void
ASagaCharacterBase::ExecuteJump()
{
	if (CanJump())
	{
		Jump();
	}
}

void
ASagaCharacterBase::TerminateJump()
{}

void
ASagaCharacterBase::ExecuteRotate(const float pitch)
{
	myCameraSpringArmComponent->AddRelativeRotation(FRotator(pitch, 0.0, 0.0));

	const FRotator Rot = myCameraSpringArmComponent->GetRelativeRotation();
	if (Rot.Pitch < -60.0)
	{
		myCameraSpringArmComponent->SetRelativeRotation(FRotator(-60.0, Rot.Yaw, Rot.Roll));
	}
	else if (Rot.Pitch > 60.0)
	{
		myCameraSpringArmComponent->SetRelativeRotation(FRotator(60.0, Rot.Yaw, Rot.Roll));
	}
}

void
ASagaCharacterBase::ExecuteGuardianAction(ASagaCharacterBase* target)
{
	StopMovement();
}

void
ASagaCharacterBase::TerminateGuardianAction()
{
	StopMovement();
}

float
ASagaCharacterBase::ExecuteHurt(const float dmg)
{
	isRunning = false;

	return myGameStat->ApplyDamage(dmg);
}

void
ASagaCharacterBase::ExecuteDeath()
{
#if WITH_EDITOR

	const auto name = GetName();
	UE_LOG(LogSagaGame, Log, TEXT("[ASagaCharacterBase::ExecuteDeath()] '%s' is dead."), *name);
#endif

	// Hide the hp bar
	if (IsValid(myHealthIndicatorBarWidget))
	{
		myHealthIndicatorBarWidget->SetHiddenInGame(true);
	}

	StopMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (OnCharacterDeath.IsBound())
	{
		OnCharacterDeath.Broadcast(this);
	}
}

void
ASagaCharacterBase::ExecuteRespawn()
{
	// Reset game states
	myGameStat->ResetHp();

	// Unhide the hp bar
	if (IsValid(myHealthIndicatorBarWidget))
	{
		myHealthIndicatorBarWidget->SetHiddenInGame(false);
	}

	// Reset the position
	FVector spawn_pos;
	FRotator spawn_rot;

	if (GetTeam() == ESagaPlayerTeam::Red)
	{
		spawn_pos = FVector{ 3100.0, 3400.0, 320.0 };
		spawn_rot = FRotator::ZeroRotator;
	}
	else // blue team
	{
		spawn_pos = FVector{ -3892.0, -3540.0, 320.0 };
		//spawn_rot = FRotator{ 0, -129.0, 0 };
		spawn_rot = FRotator::ZeroRotator;
	}

	SetActorLocationAndRotation(spawn_pos, spawn_rot);

#if WITH_EDITOR

	UE_LOG(LogSagaGame, Warning, TEXT("Character respawned at location: %s"), *spawn_pos.ToString());
#endif

	if (OnCharacterRespawned.IsBound())
	{
		OnCharacterRespawned.Broadcast(this);
	}
}
