#include "SagaCharacterPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>

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
ASagaCharacterPlayer::ExecuteAttack()
{
	PlayAttackAnimation();
}

void
ASagaCharacterPlayer::TerminateAttack()
{}

void
ASagaCharacterPlayer::ExecuteRide()
{

}

void
ASagaCharacterPlayer::TerminateRide()
{

}

float
ASagaCharacterPlayer::ExecuteHurt(const float dmg)
{
	return 0.0f;
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
