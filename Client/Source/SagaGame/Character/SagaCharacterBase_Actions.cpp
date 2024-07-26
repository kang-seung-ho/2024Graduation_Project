#include "Character/SagaCharacterBase.h"
#include <Engine/EngineTypes.h>
#include <UObject/UObjectGlobals.h>
#include <UObject/Object.h>
#include <Engine/World.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <NiagaraSystem.h>
#include <NiagaraCommon.h>
#include <NiagaraFunctionLibrary.h>
#include <NiagaraComponent.h>

#include "Player/SagaPlayerTeam.h"
#include "Item/SagaItemTypes.h"
#include "Item/Gumball.h"
#include "UI/SagaWidgetComponent.h"

void
ASagaCharacterBase::ExecuteStraightWalk(const int& direction)
noexcept
{
	straightMoveDirection = direction;
}

void
ASagaCharacterBase::ExecuteStrafeWalk(const int& direction)
noexcept
{
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

	myGameStat->RetryUpdateHealth();

	// Reset the position
	FVector spawn_pos;
	FRotator spawn_rot;

	if (GetTeam() == ESagaPlayerTeam::Red)
	{
		spawn_pos = FVector{ 20.0, 5110.0, 157.0 };
		spawn_rot = FRotator::ZeroRotator;
	}
	else // blue team
	{
		spawn_pos = FVector{ 28.0, -5080.0, 153.0 };
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

void
ASagaCharacterBase::ExecuteUseItem(ESagaItemTypes item)
{
	const auto world = GetWorld();

	switch (item)
	{
	case ESagaItemTypes::Drink:
	{
		SetHealth(GetHealth() + 30);
	}
	break;

	case ESagaItemTypes::Gum:
	{
		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 200.0f;
		SpawnLocation.Z -= 30.0f;

		FRotator SpawnRotation = GetActorRotation();
		FActorSpawnParameters SpawnParams{};
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		world->SpawnActor<AGumball>(AGumball::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
	}
	break;

	case ESagaItemTypes::SmokeBomb:
	{
		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 50.0f;
		SpawnLocation.Z += 50.0f;

		FRotator SpawnRotation = GetActorRotation();

		UNiagaraSystem* SmokeEffect = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/Item/VFX/NS_Smoke.NS_Smoke'"));
		if (SmokeEffect)
		{
			UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(world, SmokeEffect, SpawnLocation, SpawnRotation);

			if (NiagaraComponent)
			{
				NiagaraComponent->SetAutoDestroy(true);  // auto destroy after effect is done
			}
		}
	}
	break;

	default:
	{}
	break;
	}
}