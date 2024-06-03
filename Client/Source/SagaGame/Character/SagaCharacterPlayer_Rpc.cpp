#include "SagaCharacterPlayer.h"
#include <Engine/EngineTypes.h>
#include <GameFramework/CharacterMovementComponent.h>

#include "SagaInGamePlayerController.h"
#include "UI/SagaWidgetComponent.h"
#include "UI/SagaHpBarWidget.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

void
ASagaCharacterPlayer::ExecuteStraightWalk(const int& direction)
noexcept
{
	UE_LOG(LogSagaGame, Log, TEXT("[Character] ExecuteStraightWalk (%d)"), direction);

	straightMoveDirection = direction;
}

void
ASagaCharacterPlayer::ExecuteStrafeWalk(const int& direction)
noexcept
{
	UE_LOG(LogSagaGame, Log, TEXT("[Character] ExecuteStrafeWalk (%d)"), direction);

	strafeMoveDirection = direction;
}

void
ASagaCharacterPlayer::TerminateStraightWalk()
{
	straightMoveDirection = 0;
}

void
ASagaCharacterPlayer::TerminateStrafeWalk()
{
	strafeMoveDirection = 0;
}

void
ASagaCharacterPlayer::ExecuteRun()
{
	// 달리기 시작할 때 속도를 높임
	GetCharacterMovement()->MaxWalkSpeed = GetMaxMoveSpeed(true);
}

void
ASagaCharacterPlayer::TerminateRun()
{
	// 달리기를 멈췄을 때 속도를 원래대로 복원
	GetCharacterMovement()->MaxWalkSpeed = GetMaxMoveSpeed(false);
}

void
ASagaCharacterPlayer::ExecuteJump()
{
	if (CanJump())
	{
		Jump();
	}
}

void
ASagaCharacterPlayer::TerminateJump()
{}

void
ASagaCharacterPlayer::ExecuteRotate(const float pitch)
{
	mArm->AddRelativeRotation(FRotator(pitch, 0.0, 0.0));

	const FRotator Rot = mArm->GetRelativeRotation();
	if (Rot.Pitch < -60.0)
	{
		mArm->SetRelativeRotation(FRotator(-60.0, Rot.Yaw, Rot.Roll));
	}
	else if (Rot.Pitch > 60.0)
	{
		mArm->SetRelativeRotation(FRotator(60.0, Rot.Yaw, Rot.Roll));
	}
}

void
ASagaCharacterPlayer::ExecuteRide()
{
	isRunning = false;
	TerminateStraightWalk();
	TerminateStrafeWalk();
}

void
ASagaCharacterPlayer::TerminateRide()
{
	isRunning = false;
	TerminateStraightWalk();
	TerminateStrafeWalk();
}

void
ASagaCharacterPlayer::ExecuteAttack()
{
	PlayAttackAnimation();
}

void
ASagaCharacterPlayer::TerminateAttack()
{}

float
ASagaCharacterPlayer::ExecuteHurt(const float dmg)
{
	UE_LOG(LogSagaGame, Log, TEXT("[Character] ExecuteHurt (%f)"), dmg);

	return Stat->ApplyDamage(dmg);
}

void
ASagaCharacterPlayer::ExecuteDeath()
{
	isRunning = false;
	TerminateStraightWalk();
	TerminateStrafeWalk();

	GetCharacterMovement()->StopActiveMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void
ASagaCharacterPlayer::ExecuteRespawn()
{
	// Reset game states
	Stat->ResetHp();

	// Unhide the hp bar
	HpBar->SetHiddenInGame(false);

	// Reset the position
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (system->IsOfflineMode())
	{
		const FVector SpawnLocation = FVector(0, 0, 330.0f);
		const FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

		SetActorLocationAndRotation(SpawnLocation, SpawnRotation);

		UE_LOG(LogSagaGame, Warning, TEXT("Character respawned at Location: %s (Offline Mode)"), *SpawnLocation.ToString());
	}
	else if (system->IsConnected())
	{
		const auto controller = GetController<ASagaInGamePlayerController>();

		FVector spawn_pos;
		FRotator spawn_rot;

		if (myTeam == EUserTeam::Red)
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

		// TODO

		/*

		if (GetUserId() == system->GetLocalUserId())
		{
			const auto spawner = controller->GetLocalPlayerSpawner();
			spawn_pos = spawner->GetActorLocation();
			spawn_rot = spawner->GetActorRotation();
		}
		else
		{
			spawn_pos = system->GetStoredPosition(GetUserId());
			spawn_rot = FRotator::ZeroRotator;
		}
		spawn_pos += FVector{ 0, 0, 50.0 };
		*/

		//TeleportTo(spawn_pos, spawn_rot);
		SetActorLocationAndRotation(spawn_pos, spawn_rot);

		UE_LOG(LogSagaGame, Warning, TEXT("Character respawned at location: %s"), *spawn_pos.ToString());
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("[ExecuteRespawn] Network subsystem is not ready."));
	}
}
